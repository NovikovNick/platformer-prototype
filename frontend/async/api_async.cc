#include <schema.pb.h>
#include <serializer.h>
#include <util.h>

#include <bitset>
#include <chrono>
#include <iostream>
#include <thread>

#include "../api.h"
#include "core_game_loop.h"
#include "game_state.h"

namespace {
auto gs = std::make_shared<platformer::GameState>();
auto p0_input = std::make_shared<std::atomic<int>>(0);
auto p1_input = std::make_shared<std::atomic<int>>(0);

std::mutex m;
auto running = std::make_shared<std::atomic<bool>>(false);
auto stopped = true;
std::string local_public_ip = "disabled";
}  // namespace

void Init(const Location loc) {
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

void RegisterPeer(const Endpoint remote_endpoint){};

void StartGame() {
  std::scoped_lock lock(m);
  if (!running->load() && stopped) {
    running->store(true);
    stopped = false;

    std::thread([] {
      platformer::CoreGameLoop loop(gs, p0_input, p1_input, running);
      loop();
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


long long getMicrosecondsInOneTick(){
  return platformer::CoreGameLoop::getMicrosecondsInOneTick();
};

GameStatus GetStatus() {
  using namespace std::chrono_literals;
  if (running->load()) return GameStatus::RUN;
  return stopped ? GameStatus::STOPED : GameStatus::RUN;
};