#include "../api.h"

#include <schema.pb.h>
#include <serializer.h>

#include <bitset>
#include <iostream>

#include "game_state.h"

namespace {
platformer::GameState gs;
bool running = false;
std::string local_public_ip = "disabled";
}  // namespace

void Init(const Location location) {
  // args.local = location.is_1st_player;
  gs = {};
  gs.setPlayerPosition(0, location.position_1st_player.x,
                        location.position_1st_player.y);
  gs.setPlayerPosition(1, location.position_2nd_player.x,
                        location.position_2nd_player.y);
  gs.removeAllPlatforms();
  for (int i = 0; i < location.platforms_count; ++i) {
    auto& it = location.platforms[i];
    gs.addPlatform(it.width, it.height, it.position.x, it.position.y);
  }
};

Endpoint GetPublicEndpoint(const int local_port) {
  return {local_public_ip.c_str(), 0};
};

void RegisterPeer(const Endpoint remote_endpoint){};

void StartGame() {
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

PlatformerErrorCode GetErrorCode() { return PlatformerErrorCode::OK; };