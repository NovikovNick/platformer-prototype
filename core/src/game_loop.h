#ifndef GEOM_2D_MATH_GAME_LOOP_H
#define GEOM_2D_MATH_GAME_LOOP_H
#include <iostream>

#include "game_state.h"

namespace platformer {

class GameLoop {
  bool running_;
  uint64_t frame_;
  std::shared_ptr<std::atomic<int>> tick_, tick_rate_, p0_input_, p1_input_;
  std::shared_ptr<std::atomic<float>> tick_ratio_;
  std::shared_ptr<GameState> gs_;

 public:
  GameLoop(std::shared_ptr<GameState> gs,
           std::shared_ptr<std::atomic<int>> tick,
           std::shared_ptr<std::atomic<int>> tick_rate,
           std::shared_ptr<std::atomic<float>> tick_ratio,
           std::shared_ptr<std::atomic<int>> p0_input,
           std::shared_ptr<std::atomic<int>> p1_input);

  void operator()();
};

};      // namespace platformer
#endif  // GEOM_2D_MATH_GAME_LOOP_H