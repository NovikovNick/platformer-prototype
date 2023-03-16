#ifndef PLATFORMER_PLAYER_H
#define PLATFORMER_PLAYER_H
#include "game_object.h"

namespace platformer {

enum class PlayerState {
  IDLE = 0,
  RUN = 1,
  JUMP = 2,
  FALLING = 3,
  LANDING = 4,
  ATTACK_ON_GROUND = 5,
  DEATH = 6
};

class Player {
 public:
  GameObject obj;
  PlayerState state;
  uint64_t state_frame;
  int prev_input;
  bool on_ground, on_damage, look_at_left;
  Player();
  void updateState(const PlayerState state);
  bool is(const PlayerState state) const;
};

};      // namespace platformer
#endif  // PLATFORMER_PLAYER_H