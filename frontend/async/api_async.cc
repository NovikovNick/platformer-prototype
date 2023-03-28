#include "../api.h"

#include <schema.pb.h>
#include <serializer.h>

#include <bitset>
#include <chrono>
#include <iostream>
#include <thread>

#include "core_game_loop.h"
#include "game_state.h"
#include "util.h"

namespace {
auto gs = std::make_shared<platformer::GameState>();
auto tick = std::make_shared<std::atomic<int>>(0);
auto p0_input = std::make_shared<std::atomic<int>>(0);
auto p1_input = std::make_shared<std::atomic<int>>(0);
auto running = std::make_shared<std::atomic<bool>>(false);
}  // namespace

void StartGame() {
  running->store(true);
  std::thread(platformer::CoreGameLoop(gs, tick, p0_input, p1_input, running))
      .detach();
};

void StopGame() { running->store(false); };

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
