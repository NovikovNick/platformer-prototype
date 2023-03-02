#ifndef GEOM_2D_MATH_PLAYER_SHAPE_H
#define GEOM_2D_MATH_PLAYER_SHAPE_H
#include <Eigen/Dense>
#include <SFML/Graphics.hpp>
#include <vector>

#include "game_object.h"
#include "vector_shape.h"

namespace math {

class PlayerShape : public sf::Drawable {
  sf::VertexArray player_shape_;
  VectorShape velocity_vector_;
  GameObject prev_, curr_;

 public:
  PlayerShape(const sf::Color main_color, const sf::Color velocity_color,
              const GameObject curr);
  void update(const GameObject curr);
  void update(const float delta);
  void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
};      // namespace math
#endif  // GEOM_2D_MATH_PLAYER_SHAPE_H