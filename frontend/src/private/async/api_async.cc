#include <schema.pb.h>
#include <serializer.h>
#include <util.h>

#include <bitset>
#include <chrono>
#include <iostream>
#include <thread>

#include "api.h"
#include "game_loop_ticker.h"
#include "game_state.h"

namespace {
auto gs = std::make_shared<platformer::GameState>();
auto p0_input = std::make_shared<std::atomic<int>>(0);
auto p1_input = std::make_shared<std::atomic<int>>(0);

std::mutex m;
auto running = std::make_shared<std::atomic<bool>>(false);
auto stopped = true;
std::string local_public_ip = "disabled";

int tick_rate = 60;
long long micro_in_one_tick = 1;
auto on_tick = [] { gs->update(p0_input->load(), p1_input->load()); };
}  // namespace

void Init(const GameContext ctx) { tick_rate = ctx.tick_rate; };

void SetLocation(const Location loc) {
  // args.local = location.is_1st_player;
  gs = std::make_shared<platformer::GameState>();
  gs->setPlayerPosition(0, loc.position_1st_player.x, loc.position_1st_player.y);
  gs->setPlayerPosition(1, loc.position_2nd_player.x, loc.position_2nd_player.y);
  gs->removeAllPlatforms();
  for (int i = 0; i < loc.platforms_count; ++i) {
    auto &it = loc.platforms[i];
    gs->addPlatform(it.width, it.height, it.position.x, it.position.y);
  }
};

Endpoint GetPublicEndpoint(const int local_port) {
  return {local_public_ip.c_str(), 0};
};

template <int TICK_RATE>
void start() {
  platformer::GameLoopTicker<TICK_RATE> loop(on_tick, running);
  micro_in_one_tick = loop.getMicrosecondsInOneTick();
  platformer::debug("Game session started with tick rate: {}\n", TICK_RATE);
  loop();
};

void StartGame() {
  std::scoped_lock lock(m);
  if (!running->load() && stopped) {
    running->store(true);
    stopped = false;

    std::thread([] {
      switch (tick_rate) {
        case 10: start<10>(); break;
        case 20: start<20>(); break;
        case 30: start<30>(); break;
        case 40: start<40>(); break;
        case 50: start<50>(); break;
        case 60: start<60>(); break;
      }
      stopped = true;
    }).detach();
  }
};

void StopGame() {
  std::scoped_lock lock(m);
  running->store(false);
};

void Update(const Input input) {
  std::bitset<6> input_bitset;
  input_bitset[kInputLeft] = input.leftPressed;
  input_bitset[kInputRight] = input.rightPressed;
  input_bitset[kInputUp] = input.upPressed;
  input_bitset[kInputDown] = input.downPressed;
  input_bitset[kInputLKM] = input.leftMouseClicked;
  input_bitset[kInputRKM] = input.rightMouseClicked;

  p0_input->store(input_bitset.to_ullong());
};

void GetState(uint8_t *buf, int *length) {
  *length = platformer::Serializer::serialize(gs->getStateProjection(), buf);
}

long long getMicrosecondsInOneTick() { return micro_in_one_tick; };

GameStatus GetStatus() {
  using namespace std::chrono_literals;
  if (running->load()) return GameStatus::RUN;
  return stopped ? GameStatus::STOPED : GameStatus::RUN;
};