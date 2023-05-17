#include "../game_loop_ticker.h"

#include <Windows.h>

#include <thread>

#include "util.h"

namespace platformer {

using namespace std::chrono;
using clock = steady_clock;

template <int TICK_RATE>
GameLoopTicker<TICK_RATE>::GameLoopTicker(std::function<void()> on_tick,
                                          std::shared_ptr<std::atomic<bool>> running)
    : on_tick_(on_tick), tick_(0 - 1), running_(running) {
  if (timeBeginPeriod(1) == TIMERR_NOERROR) {
    debug("Minimum resolution for periodic timers has been updated to 1ms\n");
  } else {
    debug("Unable to set minimum resolution for periodic timers in windows!\n");
  }
};

template <int TICK_RATE>
GameLoopTicker<TICK_RATE>::~GameLoopTicker() {
  timeEndPeriod(1);
  debug("Minimum resolution for periodic timers has been resetted\n");
}

template <int TICK_RATE>
void GameLoopTicker<TICK_RATE>::operator()() {
  auto started_time = clock::now();
  auto current_time = started_time;
  int update_time = 0;
  int sleep_time = 0;

  microseconds running_time;
  tick ticks;
  uint64_t startup_offset;

  while (running_->load()) {
    running_time = duration_cast<microseconds>(current_time - started_time);
    ticks = duration_cast<tick>(running_time);
    startup_offset =
        running_time.count() - duration_cast<microseconds>(ticks).count();

    if (tick_ + 1 != ticks.count())
      debug("Missed frame {} => {}!\n", tick_, ticks.count());

    if (tick_ != ticks.count()) {
      tick_ = ticks.count();

      on_tick_();

      update_time = duration_cast<microseconds>(clock::now() - current_time).count();
      sleep_time = std::ceil((tick_time_ - update_time - startup_offset) / 1000.0);

      if (sleep_time > 0) Sleep(sleep_time);
      current_time = clock::now();
    } else {
      Sleep(1);
      current_time = clock::now();
      debug("Sleep Frame {} \n", tick_);
    }
  }
}

template class GameLoopTicker<10>;
template class GameLoopTicker<20>;
template class GameLoopTicker<30>;
template class GameLoopTicker<40>;
template class GameLoopTicker<50>;
template class GameLoopTicker<60>;
};  // namespace platformer