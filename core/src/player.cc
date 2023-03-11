#include "player.h"

namespace platformer {

Player::Player()
    : state_(PlayerState::IDLE),
      on_platform_(false),
      obj(GameObject(64, 128,
                     {{FIX(-0.5), FIX(0.5)},
                      {FIX(0.5), FIX(0.5)},
                      {FIX(0.5), FIX(-0.5)},
                      {FIX(-0.5), FIX(-0.5)}})){};

void Player::updateFrame(const PlayerState state) {
  frame_ = state_ != state ? 0 : frame_ + 1;
  state_ = state;
}
}  // namespace platformer
