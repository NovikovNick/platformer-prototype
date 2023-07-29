#include "collision_service.h"

#include <fpm/math.hpp>
namespace {
inline VECTOR_2 normal(const VECTOR_2& val) {
  if (val.x() == kZero && val.y() == kZero) return {kZero, kZero};
  return val / fpm::sqrt(fpm::pow(val.x(), 2) + fpm::pow(val.y(), 2));
}
}  // namespace

namespace platformer {

std::pair<FIX, FIX> CollistionService::isIntersect(const GameObject& lhs,
                                                   const GameObject& rhs) {
  auto [lhs_min_x, lhs_max_x] = lhs.getProjectionMinMax(0);
  auto [rhs_min_x, rhs_max_x] = rhs.getProjectionMinMax(0);
  auto [lhs_min_y, lhs_max_y] = lhs.getProjectionMinMax(1);
  auto [rhs_min_y, rhs_max_y] = rhs.getProjectionMinMax(1);

  // rhs_max_y >= lhs_min_y && rhs_min_y <= lhs_max_y;
  if (rhs_max_x - lhs_min_x < kZero) return {kZero, kZero};
  if (lhs_max_x - rhs_min_x < kZero) return {kZero, kZero};
  if (rhs_max_y - lhs_min_y < kZero) return {kZero, kZero};
  if (lhs_max_y - rhs_min_y < kZero) return {kZero, kZero};

  FIX diff_x = std::min(rhs_max_x - lhs_min_x, lhs_max_x - rhs_min_x);
  FIX diff_y = std::min(rhs_max_y - lhs_min_y, lhs_max_y - rhs_min_y);

  if (rhs_max_x > lhs_max_x) diff_x *= -1;
  if (rhs_max_y > lhs_max_y) diff_y *= -1;
  return {diff_x, diff_y};
}

void CollistionService::resolveCollision(GameObject& subject,
                                         const GameObject& object) {
  auto [intersection_x, intersection_y] = isIntersect(subject, object);
  if (intersection_x != kZero && intersection_y != kZero) {
    int x = std::abs(static_cast<int>(intersection_x));
    int y = std::abs(static_cast<int>(intersection_y));
    if (x <= y) subject.position.x() += intersection_x;
    if (x >= y) subject.position.y() += intersection_y;
  }
}

};  // namespace platformer