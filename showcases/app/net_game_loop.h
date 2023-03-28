#ifndef PLATFORMER_NET_GAME_LOOP_H
#define PLATFORMER_NET_GAME_LOOP_H
#include <game_state.h>

#include <iostream>

#include "input_args.h"

namespace platformer {

class NetGameLoop {
  bool running_;
  uint64_t frame_;
  std::shared_ptr<std::atomic<int>> tick_, p0_input_, p1_input_;

 public:
  NetGameLoop(InputArgs args, std::shared_ptr<GameState> gs,
              std::shared_ptr<std::atomic<int>> tick,
              std::shared_ptr<std::atomic<int>> p0_input,
              std::shared_ptr<std::atomic<int>> p1_input);
  ~NetGameLoop();
  void operator()();
};

};      // namespace platformer
#endif  // PLATFORMER_NET_GAME_LOOP_H