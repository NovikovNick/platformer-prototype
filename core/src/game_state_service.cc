#include "game_state_service.h"

namespace platformer {

GameStateService::GameStateService() : state_data(15) {
  // moveable, is_attack, damage, hit_damage;
  state_data[(int)PlayerState::IDLE] = {true, false, 0, 0};
  state_data[(int)PlayerState::RUN] = {true, false, 0, 0};
  state_data[(int)PlayerState::JUMP] = {true, false, 0, 0};
  state_data[(int)PlayerState::FALLING] = {true, false, 0, 0};
  state_data[(int)PlayerState::LANDING] = {true, false, 0, 0};
  state_data[(int)PlayerState::SQUAT] = {false, false, 0, 0};

  state_data[(int)PlayerState::OVERHEAD_ATTACK] = {false, true, 10, 2};
  state_data[(int)PlayerState::MID_ATTACK] = {false, true, 2, 1};
  state_data[(int)PlayerState::LOW_ATTACK] = {false, true, 15, 3};

  state_data[(int)PlayerState::BLOCK] = {false, false, 0, 0};
  state_data[(int)PlayerState::SQUAT_BLOCK] = {false, false, 0, 0};
  state_data[(int)PlayerState::HIT_STUN] = {false, false, 0, 0};
  state_data[(int)PlayerState::BLOCK_STUN] = {false, false, 0, 0};
  state_data[(int)PlayerState::SQUAT_BLOCK_STUN] = {false, false, 0, 0};
  state_data[(int)PlayerState::DEATH] = {false, false, 0, 0};
};

const PlayerStateData& GameStateService::get(const PlayerState state) const {
  return state_data[(int)state];
};

};  // namespace platformer
