#ifndef PLATFORMER_GAME_STATE_SERVICE_H
#define PLATFORMER_GAME_STATE_SERVICE_H

#include <functional>
#include <vector>

#include "game_object.h"
#include "player.h"

namespace platformer {

struct PlayerStateData {
  bool moveable;
  bool is_duck;
  std::function<void(Player&, GameObject&)> on_state, on_state_out;
};

struct FrameData {
  int damage;
  int hit_damage;
};

class GameStateService {
  std::vector<PlayerStateData> state_data;
  std::vector<FrameData> frame_data;

 public:
  GameStateService();
  const bool isAttack(const PlayerState state) const;
  const PlayerStateData& getStateData(const PlayerState state) const;
  const FrameData& getFrameData(const PlayerState state) const;
};

};      // namespace platformer
#endif  // PLATFORMER_GAME_STATE_SERVICE_H