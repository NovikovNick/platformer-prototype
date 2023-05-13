#include "core_game_loop.h"

#include <Windows.h>

#include <chrono>
#include <thread>

#include "util.h"

namespace platformer {

using namespace std::chrono;
using clock = steady_clock;
using frame = duration<uint64_t, std::ratio<1, 60>>;

CoreGameLoop::CoreGameLoop(std::shared_ptr<GameState> gs,
                           std::shared_ptr<std::atomic<int>> p0_input,
                           std::shared_ptr<std::atomic<int>> p1_input,
                           std::shared_ptr<std::atomic<bool>> running)
    : gs_(gs),
      frame_(0 - 1),
      p0_input_(p0_input),
      p1_input_(p1_input),
      running_(running){};

void CoreGameLoop::operator()() {
  if (timeBeginPeriod(1) == TIMERR_NOERROR) {
    debug("Minimum resolution for periodic timers has been updated to 1ms\n");
  } else {
    debug(
        "Unable to set minimum resolution for periodic timers in windows! App "
        "will work in busy loop.\n");
  }

  const int frame_time = getMicrosecondsInOneTick();
  auto started_time = clock::now();
  auto current_time = started_time;
  int update_time = 0;
  int sleep_time = 0;

  microseconds running_time;
  frame frames;
  uint64_t startup_offset;

  while (running_->load()) {
    running_time = duration_cast<microseconds>(current_time - started_time);
    frames = duration_cast<frame>(running_time);
    startup_offset =
        running_time.count() - duration_cast<microseconds>(frames).count();

    if (frame_ + 1 != frames.count())
      debug("Missed frame {} => {}!\n", frame_, frames.count());

    if (frame_ != frames.count()) {
      frame_ = frames.count();

      gs_->update(p0_input_->load(), p1_input_->load());

      update_time = duration_cast<microseconds>(clock::now() - current_time).count();
      sleep_time = std::ceil((frame_time - update_time - startup_offset) / 1000.0);

      if (sleep_time > 0) Sleep(sleep_time);
      current_time = clock::now();
    } else {
      Sleep(1);
      current_time = clock::now();
      debug("Sleep Frame {} \n", frame_);
    }
  }
  timeEndPeriod(1);
}
long long CoreGameLoop::getMicrosecondsInOneTick() {
  static auto tick = duration_cast<microseconds>(frame{1}).count();
  return tick;
};
};  // namespace platformer