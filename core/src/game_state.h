#ifndef GEOM_2D_MATH_GAME_STATE_H
#define GEOM_2D_MATH_GAME_STATE_H
#include <Eigen/Dense>
#include <fpm/fixed.hpp>
#include <future>
#include <vector>

#include "game_object.h"
#include "player.h"

namespace platformer {

class GameState {
  std::mutex mutex_;
  std::vector<Player> players_;
  std::vector<GameObject> platforms_;

 public:
  GameState();
  void update(const int p0_input, const int p1_input, const int frames);
  GameObject getPlayer(const int player_id);
  std::vector<GameObject>& getPlatforms();

 private:
  bool checkPlatform(const int player_id);
};

};      // namespace platformer
#endif  // GEOM_2D_MATH_GAME_STATE_H