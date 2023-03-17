#ifndef PLATFORMER_GAME_OBJECT_H
#define PLATFORMER_GAME_OBJECT_H
#include <Eigen/Dense>
#include <fpm/fixed.hpp>
#include <vector>

using FIX = fpm::fixed_16_16;
using VECTOR_2 = Eigen::Vector2<FIX>;
using VECTOR_3 = Eigen::Vector3<FIX>;
namespace {
constexpr int sum(const int n) {
  int res = 0;
  for (int i = 1; i <= n; ++i) res += i;
  return res;
}

const static FIX kZero{0};
const static FIX kOne{1};

const static int kInputLeft = 0;
const static int kInputRight = 1;
const static int kInputUp = 2;
const static int kInputDown = 3;
const static int kInputLKM = 4;

const static int kPlayerCount = 2;

// braking:               4  frame
const static int kJumpHeight = 128;
const static int kJump = 12;
const static int kAttack = 6;
const static int kAccelerationX = 1;
const static float kAccelerationGravity = 1.5f;
const static int kMaxVelocityX = 12;
const static int kMaxVelocityFall = 20;
const static FIX kJumpDelta = FIX{kJumpHeight} / FIX{sum(kJump)};

}  // namespace

namespace platformer {

class GameObject {
 public:
  std::vector<VECTOR_2> mesh;
  int width_, height_;
  VECTOR_2 position, velocity;

  GameObject(const int width, const int height,
             const std::vector<VECTOR_2>& mesh);

  VECTOR_3 operator[](const size_t index) const;

  std::pair<FIX, FIX> getProjectionMinMax(const int axis) const;

  size_t size() const;
};

};      // namespace platformer
#endif  // PLATFORMER_GAME_OBJECT_H