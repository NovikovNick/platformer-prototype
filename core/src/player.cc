#include "player.h"

namespace platformer {

Player::Player()
    : state(PlayerState::IDLE),
      state_frame(0),
      attack_phase(AttackPhase::NONE),
      prev_input(0),
      on_ground(false),
      on_damage(false),
      left_direction(false),
      current_health(100),
      max_health(100),
      obj(GameObject(64, 128,
                     {{FIX(0), FIX(1)},
                      {FIX(1), FIX(1)},
                      {FIX(1), FIX(0)},
                      {FIX(0), FIX(0)}})){};

void Player::updateState(const PlayerState new_state) {
  if (state != new_state) {
    state = new_state;
    state_frame = 0;
  }
};
bool Player::is(const PlayerState other_state) const {
  return state == other_state;
};
}  // namespace platformer
