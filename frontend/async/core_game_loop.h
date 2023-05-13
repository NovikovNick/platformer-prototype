#ifndef PLATFORMER_CORE_GAME_LOOP_H
#define PLATFORMER_CORE_GAME_LOOP_H
#include <iostream>

#include "game_state.h"

namespace platformer {

class CoreGameLoop {
  uint64_t frame_;
  std::shared_ptr<std::atomic<int>> tick_, p0_input_, p1_input_;
  std::shared_ptr<std::atomic<bool>> running_;
  std::shared_ptr<GameState> gs_;

 public:
  CoreGameLoop(std::shared_ptr<GameState> gs,
               std::shared_ptr<std::atomic<int>> p0_input,
               std::shared_ptr<std::atomic<int>> p1_input,
               std::shared_ptr<std::atomic<bool>> running);

  void operator()();

  static long long getMicrosecondsInOneTick();
};

};      // namespace platformer
#endif  // PLATFORMER_CORE_GAME_LOOP_H