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
                           std::shared_ptr<std::atomic<int>> tick,
                           std::shared_ptr<std::atomic<int>> p0_input,
                           std::shared_ptr<std::atomic<int>> p1_input,
                           std::shared_ptr<std::atomic<bool>> running)
    : gs_(gs),
      frame_(0 - 1),
      tick_(tick),
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

  const int frame_time = getMicrosecondsInOneFrame();
  auto started_time = clock::now();
  auto current_time = started_time;
  int update_time = 0;
  int sleep_time = 0;
  uint64_t frame_started_at = 0;

  microseconds running_time;
  frame frames;
  uint64_t frame_startup_offset;

  while (running_->load()) {
    running_time = duration_cast<microseconds>(current_time - started_time);
    frames = duration_cast<frame>(running_time);
    frame_startup_offset =
        running_time.count() - duration_cast<microseconds>(frames).count();
    frame_started_at = duration_cast<microseconds>(
                            duration_cast<frame>(current_time.time_since_epoch()))
                            .count();

    if (frame_ + 1 != frames.count())
      debug("Missed frame {} => {}!\n", frame_, frames.count());

    if (frame_ != frames.count()) {
      frame_ = frames.count();
      tick_->store(frame_);
      gs_->update(p0_input_->load(), p1_input_->load());
      gs_->timestamp = frame_started_at;
      update_time = duration_cast<microseconds>(clock::now() - current_time).count();
      sleep_time =
          std::ceil((frame_time - update_time - frame_startup_offset) / 1000.0);

      debug("frame {:4d}, started_at {:15d}, update_time {:4d}, sleep_time {:d}!\n",
            frame_,
            frame_started_at,
            update_time,
            frame_time - update_time - frame_startup_offset);

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
uint64_t CoreGameLoop::getMicrosecondsInOneFrame() {
  return duration_cast<microseconds>(frame{1}).count();
};
};  // namespace platformer