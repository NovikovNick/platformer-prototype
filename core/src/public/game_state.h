#ifndef PLATFORMER_GAME_STATE_H
#define PLATFORMER_GAME_STATE_H

#include <game_state_service.h>

#include <Eigen/Dense>
#include <fpm/fixed.hpp>
#include <mutex>
#include <vector>

#include "game_object.h"
#include "player.h"

namespace platformer {

class GameState {
  std::mutex mutex_;
  std::vector<VECTOR_2> left_top_mesh_;
  GameStateService state_service_;

 public:
  int frame;
  std::vector<Player> players_;
  std::vector<GameObject> platforms_;
  std::vector<GameObject> melee_attack;

  GameState();
  GameState(GameState& src);

  /// <summary>
  /// method to update all game state
  /// </summary>
  /// <param name="p0_input">player 1 input</param>
  /// <param name="p1_input">player 2 input</param>
  void update(const int p0_input, const int p1_input);

  // setup location
  void removeAllPlatforms();
  void addPlatform(const int width, const int height, const int x, const int y);
  void setPlayerPosition(const int player_id, const int x, const int y);

  GameObject getPlayer(const int player_id);
  std::vector<GameObject>& getPlatforms();

  GameState getStateProjection();
  std::unique_lock<std::mutex> lock();

  GameState& operator=(const GameState&);

 private:
  void calculateVelocity(const int player_id, const int player_input);
  void updatePlayerState(const int player_id, const int player_input);
  bool isPlayerOnGround(const int player_id);
  void resolveDamage(const int player_id);
};

};      // namespace platformer
#endif  // PLATFORMER_GAME_STATE_H