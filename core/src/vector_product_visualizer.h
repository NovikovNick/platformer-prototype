#ifndef GEOM_2D_MATH_VECTOR_PRODUCT_VISUALIZER_H
#define GEOM_2D_MATH_VECTOR_PRODUCT_VISUALIZER_H
#include <Eigen/Dense>
#include <SFML/Graphics.hpp>
#include <cmath>

#include "vector_shape.h"

namespace math {

class VectorProductVisualizer : public sf::Drawable {
  float dot_, cross_, angle_rad_, cos_, sin_, lhs_length_, rhs_length_;

  sf::Text lhs_label_, rhs_label_, quart1_label_, quart2_label_, quart3_label_,
      quart4_label_;

  sf::VertexArray cos_projection_line_, sin_projection_line_,
      rhs_perpendicular_line_, rhs_inverted_line_;

  sf::CircleShape origin_point_;

  sf::CircleShape selected_;

  sf::CircleShape unit_circle_;

  VectorShape lhs_vector_, rhs_vector_;

  sf::VertexArray angle_arc_;

  bool drag_, lhs_selected_, rhs_selected_, origin_selected_;

 public:
  Eigen::Vector2f lhs_, rhs_, origin_;

  VectorProductVisualizer(const sf::Font& font, const sf::Color bg_color,
                          const sf::Color fst_color, const sf::Color snd_color);

  void update(const Eigen::Vector2f mouse_position,
              const bool lft_mouse_pressed);

  void update(const Eigen::Vector2f mouse_position);

  void update(Eigen::Vector2f lhs, Eigen::Vector2f rhs,
              const Eigen::Vector2f origin);

  void draw(sf::RenderTarget& target, sf::RenderStates states) const;

  float dot() const;
  float cross() const;
  float angleDegree() const;
  float cos() const;
  float sin() const;
  float lhsLength() const;
  float rhsLength() const;
  float lhsAtan2() const;
  float rhsAtan2() const;
};

};      // namespace math
#endif  // GEOM_2D_MATH_VECTOR_PRODUCT_VISUALIZER_H