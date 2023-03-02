#ifndef GEOM_2D_MATH_OBJECT_H
#define GEOM_2D_MATH_OBJECT_H
#include <Eigen/Dense>
#include <fpm/fixed.hpp>
#include <vector>

using FIXED = fpm::fixed_16_16;
using VECTOR_2 = Eigen::Vector2<FIXED>;
using VECTOR_3 = Eigen::Vector3<FIXED>;
namespace {
constexpr int sum(const int n) {
  int res = 0;
  for (int i = 1; i <= n; ++i) res += i;
  return res;
}

const static FIXED kZero{0};
const static FIXED kOne{1};

const static int kInputLeft = 0;
const static int kInputRight = 1;
const static int kInputUp = 2;
const static int kInputDown = 3;

// braking:               4  frame
const static int kJumpHeight = 128;
const static int kJump = 12;
const static int kAccelerationX = 1;
const static float kAccelerationGravity = 1.5f;
const static int kMaxVelocityX = 12;
const static int kMaxVelocityFall = 20;
const static FIXED kJumpDelta = FIXED{kJumpHeight} / FIXED{sum(kJump)};

}  // namespace

namespace math {

class GameObject {
  std::vector<VECTOR_2> mesh;
  int width_, height_;

 public:
  VECTOR_2 position, velocity;

  GameObject(const int width, const int height,
             const std::vector<VECTOR_2>& mesh);

  VECTOR_3 operator[](const size_t index) const;

  std::pair<FIXED, FIXED> getProjectionMinMax(const int axis) const;

  size_t size() const;
};

};      // namespace math
#endif  // GEOM_2D_MATH_OBJECT_H