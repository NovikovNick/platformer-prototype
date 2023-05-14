#ifndef PLATFORMER_GAME_LOOP_TICKER_H
#define PLATFORMER_GAME_LOOP_TICKER_H
#include <stdint.h>

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>

#include "game_state.h"

namespace platformer {

template <int TICK_RATE>
class GameLoopTicker {
  using tick = std::chrono::duration<uint64_t, std::ratio<1, TICK_RATE>>;
  const long long tick_time_ =
      std::chrono::duration_cast<std::chrono::microseconds>(tick{1}).count();
  uint64_t tick_;
  std::shared_ptr<std::atomic<bool>> running_;
  std::function<void()> on_tick_;

 public:
  GameLoopTicker(std::function<void()> on_tick,
               std::shared_ptr<std::atomic<bool>> running);

  void operator()();

  long long getMicrosecondsInOneTick() const { return tick_time_; }
};

};      // namespace platformer
#endif  // PLATFORMER_GAME_LOOP_TICKER_H