#include "../api.h"

#include <serializer.h>

#include <bitset>
#include <iostream>

#include "game_state.h"

namespace {
platformer::GameState gs;
}

void StartGame() { gs = platformer::GameState(); };

void StopGame(){
    // do nothing
};

void Update(const Input input) {
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
