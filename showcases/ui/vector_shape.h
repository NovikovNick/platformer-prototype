#ifndef PLATFORMER_UI_VECTOR_SHAPE_H
#define PLATFORMER_UI_VECTOR_SHAPE_H
#include <Eigen/Dense>
#include <SFML/Graphics.hpp>
#include <vector>

namespace platformer {

class VectorShape : public sf::Drawable {
  Eigen::Vector2f origin_, position_;
  std::vector<Eigen::Vector2f> arrow_shape_;
  sf::Color color_;
  sf::VertexArray arrow_array_, line_array_;

 public:
  VectorShape(sf::Color color);
  void update(const Eigen::Vector2f origin, const Eigen::Vector2f position);
  void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
};      // namespace platformer
#endif  // PLATFORMER_UI_VECTOR_SHAPE_H