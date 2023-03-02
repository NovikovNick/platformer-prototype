#ifndef GEOM_2D_MATH_PLAYER_H
#define GEOM_2D_MATH_PLAYER_H
#include "game_object.h"

namespace math {

enum class PlayerState { IDLE, RUN, JUMP_UP, JUMP_DOWN };

class Player {
 public:
  GameObject obj;
  PlayerState state_;
  uint64_t frame_;
  bool on_platform_;
  // direction left/right
  Player();
  void updateFrame(const PlayerState state);
};

};      // namespace math
#endif  // GEOM_2D_MATH_PLAYER_H