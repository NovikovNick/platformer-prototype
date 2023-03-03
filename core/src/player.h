#ifndef GEOM_2D_MATH_PLAYER_H
#define GEOM_2D_MATH_PLAYER_H
#include "game_object.h"

namespace platformer {

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

};      // namespace platformer
#endif  // GEOM_2D_MATH_PLAYER_H