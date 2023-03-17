#include "player.h"

namespace platformer {

Player::Player()
    : state(PlayerState::IDLE),
      state_frame(0),
      prev_input(0),
      on_ground(false),
      on_damage(false),
      left_direction(false),
      obj(GameObject(64, 128,
                     {{FIX(-0.5), FIX(0.5)},
                      {FIX(0.5), FIX(0.5)},
                      {FIX(0.5), FIX(-0.5)},
                      {FIX(-0.5), FIX(-0.5)}})){};

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
