#include <schema.pb.h>
#include <serializer.h>

#include <bitset>
#include <chrono>
#include <iostream>
#include <thread>

#include "../api.h"
#include "game_state.h"
#include "net_game_loop.h"
#include "util.h"

namespace {
platformer::NetGameLoop* gl;
auto gs = std::make_shared<platformer::GameState>();

auto tick = std::make_shared<std::atomic<int>>(0);
auto p0_input = std::make_shared<std::atomic<int>>(0);
auto p1_input = std::make_shared<std::atomic<int>>(0);
auto running = std::make_shared<std::atomic<bool>>(false);
InputArgs args;
}  // namespace

void RegisterPeer(int local_port, bool is_master, const char* remote_host,
                  int remote_port) {
  args.local = is_master;
  args.local_port = local_port;
  args.remote_port = remote_port;

  std::string host(remote_host);
  for (int i = 0; i < 32; ++i)
    args.ip[i] = i < host.size() ? remote_host[i] : '\0';
};

void StartGame() {
  running->store(true);
  gl = new platformer::NetGameLoop(args, gs, tick, p0_input, p1_input, running);
  std::thread(*gl).detach();
};

void StopGame() {
  running->store(false);
  delete gl;
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
