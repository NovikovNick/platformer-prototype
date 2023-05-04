#ifndef PLATFORMER_GAME_STATE_H
#define PLATFORMER_GAME_STATE_H
#include <locomotion_fsm.h>

#include <Eigen/Dense>
#include <fpm/fixed.hpp>
#include <mutex>
#include <vector>

#include "game_object.h"
#include "player.h"

namespace platformer {

class GameState {
  std::mutex mutex_;
  std::vector<PlayerSM> fsms_;

  std::vector<VECTOR_2> left_top_mesh_;

 public:
  int frame;
  std::vector<Player> players_;
  std::vector<GameObject> platforms_;
  std::vector<GameObject> melee_attack;

  GameState();
  GameState(GameState& src);

  // setup location
  void removeAllPlatforms();
  void addPlatform(const int width, const int height, const int x, const int y);
  void setPlayerPosition(const int player_id, const int x, const int y);
  void refreshStateMachine();

  void update(const int p0_input, const int p1_input, const int frames);
  GameObject getPlayer(const int player_id);
  std::vector<GameObject>& getPlatforms();

  GameState getStateProjection();
  std::unique_lock<std::mutex> lock();

  GameState& operator=(const GameState&);

 private:
  void resolveDamage(const int player_id);
  bool checkPlatform(const int player_id);
  void updateAttackPhase(const int player_id, const int startup_frame,
                         const int active_frame, const int length);
  void updatePlayerState(const int player_id, const int player_input);
};

};      // namespace platformer
#endif  // PLATFORMER_GAME_STATE_H