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
  SQUAT = 5,
  OVERHEAD_ATTACK = 6,
  MID_ATTACK = 7,
  LOW_ATTACK = 8,
  BLOCK = 9,
  SQUAT_BLOCK = 10,
  HIT_STUN = 11,
  BLOCK_STUN = 12,
  SQUAT_BLOCK_STUN = 13,
  DEATH = 14
};

enum class AttackPhase { STARTUP = 0, ACTIVE = 1, RECOVERY = 2, NONE = 3 };

class Player {
 public:
  GameObject obj;
  PlayerState state;
  uint64_t state_frame;
  AttackPhase attack_phase;
  int prev_input;
  int current_health, max_health;
  bool on_ground, on_damage, left_direction;
  Player();
  void updateState(const PlayerState state);
  bool is(const PlayerState state) const;
  bool isCrouch() const;
};

};      // namespace platformer
#endif  // PLATFORMER_PLAYER_H