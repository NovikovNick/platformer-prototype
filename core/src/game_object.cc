#include "game_object.h"

#include <fpm/fixed.hpp>  // For fpm::fixed_16_16
#include <fpm/ios.hpp>    // For fpm::operator<<
#include <fpm/math.hpp>   // For fpm::cos

#include "util.h"
namespace math {

using namespace Eigen;
using namespace fpm;

GameObject::GameObject(const int width, const int height,
                       const std::vector<VECTOR_2>& mesh)
    : width_(width),
      height_(height),
      mesh(mesh),
      position(VECTOR_2{kZero, kZero}),
      velocity(VECTOR_2{kZero, kZero}) {}

VECTOR_3 GameObject::operator[](const size_t index) const {
  Matrix3<FIXED> transition;
  transition << kOne,   kZero,  position.x(),
                kZero,  kOne,   position.y(),
                kZero,  kZero,  kOne;
  Matrix3<FIXED> scale;
  scale <<  FIXED{width_},  kZero,          kZero,
            kZero,          FIXED{height_}, kZero,
            kZero,          kZero,          kOne;
  Vector3<fixed_16_16> vect(mesh[index].x(), mesh[index].y(), kOne);
  return transition * scale * vect;
}

std::pair<FIXED, FIXED> GameObject::getProjectionMinMax(const int axis) const {
  auto extract = [](const int axis, const VECTOR_3& point) {
    return axis == 0 ? point.x() : point.y();
  };
  FIXED min = extract(axis, (*this)[0]);
  FIXED max = min;
  for (int i = 1; i < mesh.size(); ++i) {
    min = std::min(extract(axis, (*this)[i]), min);
    max = std::max(extract(axis, (*this)[i]), max);
  }
  return {min, max};
}

size_t GameObject::size() const { return mesh.size(); }

}  // namespace math