#ifndef PLATFORMER_GAME_STATE_SERVICE_H
#define PLATFORMER_GAME_STATE_SERVICE_H

#include <vector>

#include "player.h"

namespace platformer {

struct PlayerStateData {
  bool moveable;
  bool is_attack;
  int damage;
  int hit_damage;
};

class GameStateService {
  std::vector<PlayerStateData> state_data;

 public:
  GameStateService();
  const PlayerStateData& get(const PlayerState state) const;
};

};      // namespace platformer
#endif  // PLATFORMER_GAME_STATE_SERVICE_H