#ifndef PLATFORMER_UTIL_H
#define PLATFORMER_UTIL_H

#include <format>
#include <iostream>
#include <string>

namespace platformer {

template <typename... Args>
void debug(const std::string& str, Args... args) {
  std::cout << std::vformat(str, std::make_format_args(args...));
}

int fletcher32_checksum(short* data, size_t len);

};      // namespace platformer
#endif  // PLATFORMER_UTIL_H