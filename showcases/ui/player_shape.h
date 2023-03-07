#ifndef PLATFORMER_UI_PLAYER_SHAPE_H
#define PLATFORMER_UI_PLAYER_SHAPE_H
#include <Eigen/Dense>
#include <SFML/Graphics.hpp>
#include <vector>

#include "game_object.h"
#include "vector_shape.h"

namespace platformer {

class PlayerShape : public sf::Drawable {
  sf::VertexArray player_shape_;
  VectorShape velocity_vector_;
  platformer::GameObject prev_, curr_;

 public:
  PlayerShape(const sf::Color main_color, const sf::Color velocity_color,
              const platformer::GameObject curr);
  void update(const platformer::GameObject curr);
  void update(const float delta);
  void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
};      // namespace platformer
#endif  // PLATFORMER_UI_PLAYER_SHAPE_H