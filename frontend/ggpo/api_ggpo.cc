#include <game_loop_ticker.h>
#include <schema.pb.h>
#include <serializer.h>
#include <util.h>

#include <bitset>
#include <chrono>
#include <iostream>
#include <thread>

#include "../api.h"
#include "game_state.h"
#include "input_args.h"
// clang-format off
#include "network/exception.h"
#include "network/stun_client.h"
#include "network/udp_socket.h"
#include "ggpo_client.h"
// clang-format on

namespace {
auto gs = std::make_shared<platformer::GameState>();
auto p0_input = std::make_shared<std::atomic<int>>(0);
auto p1_input = std::make_shared<std::atomic<int>>(0);
InputArgs args;

std::mutex m;
auto running = std::make_shared<std::atomic<bool>>(false);
auto stopped = true;
auto status = std::make_shared<std::atomic<int>>(2);

PlatformerErrorCode error_code = PlatformerErrorCode::OK;
std::string mapped_public_ip;

int tick_rate = 60;
long long micro_in_one_tick = 1;
}  // namespace

void Init(const GameContext ctx) {
  tick_rate = ctx.tick_rate;

  args.remote_port = ctx.peer_endpoint.remote_port;
  memset(args.ip, '\0', 32);
  strcpy(args.ip, ctx.peer_endpoint.remote_host);
};

void SetLocation(const Location loc) {
  args.local = loc.is_1st_player;
  gs = std::make_shared<platformer::GameState>();
  gs->setPlayerPosition(0, loc.position_1st_player.x, loc.position_1st_player.y);
  gs->setPlayerPosition(1, loc.position_2nd_player.x, loc.position_2nd_player.y);
  gs->removeAllPlatforms();
  for (int i = 0; i < loc.platforms_count; ++i) {
    auto& it = loc.platforms[i];
    gs->addPlatform(it.width, it.height, it.position.x, it.position.y);
  }
};

Endpoint GetPublicEndpoint(const int local_port) {
  try {
    args.local_port = local_port;
    net::UdpSocket socket(args.local_port);  // RAII socket
    net::StunClient stun_client(socket);
    net::Endpoint stun_server{"64.233.163.127", 19302};  // stun.l.google.com
    auto [public_ip, public_port] = stun_client.getMappedAddress(stun_server);
    mapped_public_ip = public_ip;
    return {mapped_public_ip.c_str(), public_port};
  } catch (const net::StunResponseParseException& e) {
    error_code = PlatformerErrorCode::UNABLE_TO_PARSE_STUN_RESPONSE;
  } catch (...) {
    error_code = PlatformerErrorCode::UNEXPECTED_ERROR;
  }
  return {mapped_public_ip.c_str(), 0};
};

template <int TICK_RATE>
void start() {
  platformer::UpdateHolder holder;
  platformer::GameLoopTicker<TICK_RATE> loop(
      [&holder = holder] {
        auto input = (args.local ? p0_input : p1_input)->load();
        if (holder.update(input, 8)) status->store(0);  // sleep_time = 10
      },
      running);
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
      platformer::startGameSession(args, gs);
      status->store(1);
      platformer::debug("Connecting to peers\n");

      switch (tick_rate) {
        case 10: start<10>(); break;
        case 20: start<20>(); break;
        case 30: start<30>(); break;
        case 40: start<40>(); break;
        case 50: start<50>(); break;
        case 60: start<60>(); break;
      }

      platformer::stopGameSession();
      status->store(2);
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
  (args.local ? p0_input : p1_input)->store(input_bitset.to_ullong());
};

void GetState(uint8_t* buf, int* length) {
  *length = platformer::Serializer::serialize(gs->getStateProjection(), buf);
}

long long getMicrosecondsInOneTick() { return micro_in_one_tick; };

GameStatus GetStatus() {
  switch (status->load()) {
    case 0: return GameStatus::RUN;
    case 1: return GameStatus::SYNC;
    case 2: return GameStatus::STOPED;
    default: return GameStatus::INVALID;
  }
};

PlatformerErrorCode GetErrorCode() { return error_code; };
