#include "vector_shape.h"

namespace math {

VectorShape::VectorShape(sf::Color color)
    : origin_({0, 0}),
      position_({0, 0}),
      color_(color),
      arrow_shape_({{-1.f, 0.3f}, {0.f, 0.f}, {-1.f, -0.3f}}),
      arrow_array_(sf::VertexArray(sf::Triangles, 3)),
      line_array_(sf::VertexArray(sf::LinesStrip, 2)) {
  for (int i = 0; i < 3; ++i) arrow_array_[i].color = color_;
  for (int i = 0; i < 2; ++i) line_array_[i].color = color_;
};

void VectorShape::update(const Eigen::Vector2f origin,
                         const Eigen::Vector2f position) {
  origin_ = origin;
  position_ = position;
  
  Eigen::Transform<float, 2, Eigen::Affine> t;
  t = Eigen::Matrix3f::Identity();
  t *= Eigen::Translation2f(position_);
  t *= Eigen::Scaling(20.f, 20.f);
  t *= Eigen::Rotation2Df(
      std::atan2((position_ - origin_).y(), (position_ - origin_).x()));

  for (int i = 0; i < 3; ++i) {
    auto p = t * arrow_shape_[i];
    arrow_array_[i].position = {p.x(), p.y()};
  }

  line_array_[0].position = {origin_.x(), origin_.y()};
  line_array_[1].position = {position_.x(), position_.y()};
};

void VectorShape::draw(sf::RenderTarget& target,
                       sf::RenderStates states) const {
  target.draw(line_array_, states);
  target.draw(arrow_array_, states);
};
};  // namespace math
