#ifndef GEOM_2D_MATH_UTIL_H
#define GEOM_2D_MATH_UTIL_H

#include <format>
#include <iostream>
#include <string>

namespace math {

template <typename... Args>
void debug(const std::string& str, Args... args) {
  std::cout << std::vformat(str, std::make_format_args(args...));
}

};      // namespace math
#endif  // GEOM_2D_MATH_UTIL_H