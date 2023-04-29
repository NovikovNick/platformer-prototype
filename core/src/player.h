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

enum class AttackPhase{
  STARTUP = 0,
  ACTIVE = 1,
  RECOVERY = 2
};

class Player {
 public:
  GameObject obj;
  PlayerState state;
  uint64_t state_frame;
  int prev_input;
  int current_health, max_health;
  bool on_ground, on_damage, left_direction;
  Player();
  void updateState(const PlayerState state);
  bool is(const PlayerState state) const;
};

};      // namespace platformer
#endif  // PLATFORMER_PLAYER_H