#ifndef PLATFORMER_GAME_STATE_H
#define PLATFORMER_GAME_STATE_H
#include <Eigen/Dense>
#include <fpm/fixed.hpp>
#include <future>
#include <vector>

#include "game_object.h"
#include "player.h"

namespace platformer {

class GameState {
  std::mutex mutex_;

 public:
  std::vector<Player> players_;
  std::vector<GameObject> platforms_;

  GameState();
  void update(const int p0_input, const int p1_input, const int frames);
  GameObject getPlayer(const int player_id);
  std::vector<GameObject>& getPlatforms();
  
  std::unique_lock<std::mutex> lock();

 private:
  bool checkPlatform(const int player_id);
};

};      // namespace platformer
#endif  // PLATFORMER_GAME_STATE_H