#include <schema.pb.h>
#include <serializer.h>

#include <bitset>
#include <chrono>
#include <iostream>
#include <thread>

#include "../api.h"
#include "core_game_loop.h"
#include "game_state.h"
#include "util.h"

namespace {
auto gs = std::make_shared<platformer::GameState>();
auto tick = std::make_shared<std::atomic<int>>(0);
auto p0_input = std::make_shared<std::atomic<int>>(0);
auto p1_input = std::make_shared<std::atomic<int>>(0);

std::mutex m;
auto running = std::make_shared<std::atomic<bool>>(false);
auto stopped = true;
std::string local_public_ip = "disabled";
}  // namespace

void Init(const bool is_1st_player){};

Endpoint GetPublicEndpoint(const int local_port) {
  return {local_public_ip.c_str(), 0};
};

void RegisterPeer(const Endpoint remote_endpoint){};

void StartGame() {
  std::scoped_lock lock(m);
  if (!running->load() && stopped) {
    running->store(true);
    stopped = false;
    gs = std::make_shared<platformer::GameState>();
    std::thread([] {
      platformer::CoreGameLoop loop(gs, tick, p0_input, p1_input, running);
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

  p0_input->store(input_bitset.to_ullong());
};

int GetState(uint8_t* buf) {
  return platformer::Serializer::serialize(gs->getStateProjection(), buf);
}

GameStatus GetStatus() {
  using namespace std::chrono_literals;
  if (running->load()) return GameStatus::RUN;
  return stopped ? GameStatus::STOPED : GameStatus::RUN;
};