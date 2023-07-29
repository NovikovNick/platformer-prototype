#ifndef PLATFORMER_SHOWCASE_CALLBACK_H
#define PLATFORMER_SHOWCASE_CALLBACK_H
#include <functional>

namespace platformer {
struct ShowcaseCallback {
  std::function<void()> on_restart_handler;
};
};      // namespace platformer
#endif  // PLATFORMER_SHOWCASE_CALLBACK_H