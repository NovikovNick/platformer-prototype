#include <schema.pb.h>
#include <serializer.h>

#include <bitset>
#include <chrono>
#include <iostream>
#include <thread>

#include "../api.h"
#include "game_state.h"
#include "net_game_loop.h"
#include "network/exception.h"
#include "network/stun_client.h"
#include "network/udp_socket.h"
#include "util.h"

namespace {
auto gs = std::make_shared<platformer::GameState>();

auto tick = std::make_shared<std::atomic<int>>(0);
auto p0_input = std::make_shared<std::atomic<int>>(0);
auto p1_input = std::make_shared<std::atomic<int>>(0);
InputArgs args;

std::mutex m;
auto running = std::make_shared<std::atomic<bool>>(false);
auto stopped = true;
auto status = std::make_shared<std::atomic<int>>(2);

PlatformerErrorCode error_code = PlatformerErrorCode::OK;
std::string mapped_public_ip;
}  // namespace

void Init(const bool is_1st_player) { args.local = is_1st_player; };

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

void RegisterPeer(const Endpoint remote_endpoint) {
  args.remote_port = remote_endpoint.remote_port;
  memset(args.ip, '\0', 32);
  strcpy(args.ip, remote_endpoint.remote_host);
};

void StartGame() {
  std::scoped_lock lock(m);
  if (!running->load() && stopped) {
    running->store(true);
    stopped = false;
    gs = std::make_shared<platformer::GameState>();
    std::thread([] {
      platformer::NetGameLoop loop(args, gs, tick, p0_input, p1_input, running,
                                   status);
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
  std::bitset<5> input_bitset;
  input_bitset[kInputLeft] = input.leftPressed;
  input_bitset[kInputRight] = input.rightPressed;
  input_bitset[kInputUp] = input.upPressed;
  input_bitset[kInputDown] = input.downPressed;
  input_bitset[kInputLKM] = input.leftMouseClicked;
  if (args.local) {
    p0_input->store(input_bitset.to_ullong());
  } else {
    p1_input->store(input_bitset.to_ullong());
  }
};

int GetState(uint8_t* buf) {
  return platformer::Serializer::serialize(gs->getStateProjection(), buf);
}

GameStatus GetStatus() {
  switch (status->load()) {
    case 0:
      return GameStatus::RUN;
    case 1:
      return GameStatus::SYNC;
    case 2:
      return GameStatus::STOPED;
    default:
      return GameStatus::INVALID;
  }
};

PlatformerErrorCode GetErrorCode() { return error_code; };
