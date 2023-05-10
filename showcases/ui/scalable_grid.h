#ifndef PLATFORMER_UI_SCALABLE_GRID_H
#define PLATFORMER_UI_SCALABLE_GRID_H
#include <Eigen/Dense>
#include <SFML/Graphics.hpp>
#include <vector>

namespace platformer {
class ScalableGrid : public sf::Drawable {
  sf::VertexArray grid_;
  float unit_;

 public:
  ScalableGrid(const int unit);
  void update(const int delta);
  void draw(sf::RenderTarget& target, sf::RenderStates states) const;

 private:
  static void init(sf::VertexArray& grid_, const float unit);
};
};      // namespace platformer
#endif  // PLATFORMER_UI_SCALABLE_GRID_H