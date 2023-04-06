#include "../api.h"

#include <schema.pb.h>
#include <serializer.h>

#include <bitset>
#include <iostream>

#include "game_state.h"

namespace {
platformer::GameState gs;
bool running = false;
}  // namespace

void RegisterPeer(int local_port, bool is_master, const char* remote_host,
                  int remote_port) {
  throw new std::runtime_error("RegisterPeer is unsupported for async version");
};

void StartGame() {
  gs = platformer::GameState();
  running = true;
};

void StopGame() { running = false; };

void Update(const Input input) {
  if (!running) return;

  std::bitset<5> input_bitset;
  input_bitset[kInputLeft] = input.leftPressed;
  input_bitset[kInputRight] = input.rightPressed;
  input_bitset[kInputUp] = input.upPressed;
  input_bitset[kInputDown] = input.downPressed;
  input_bitset[kInputLKM] = input.leftMouseClicked;
  gs.update(input_bitset.to_ullong(), 0, 1);
};

int GetState(uint8_t* buf) {
  return platformer::Serializer::serialize(gs, buf);
}

GameStatus GetStatus() {
  return running ? GameStatus::RUN : GameStatus::STOPED;
};