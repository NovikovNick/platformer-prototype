#ifndef PLATFORMER_UI_SCALABLE_GRID_H
#define PLATFORMER_UI_SCALABLE_GRID_H
#include <Eigen/Dense>
#include <SFML/Graphics.hpp>
#include <vector>

namespace platformer {
class ScalableGrid : public sf::Drawable {
  sf::VertexArray grid_;
  double scale_;
  int cell_count_, cell_size_px_, screen_offset_x_, screen_offset_y_;

 public:
  ScalableGrid(int cell_size_px, int cell_count);

  void update(double scale, int screen_offset_x, int screen_offset_y);

  void draw(sf::RenderTarget& target, sf::RenderStates states) const;

 private:
  void updateGridCoordinate();
};
};      // namespace platformer
#endif  // PLATFORMER_UI_SCALABLE_GRID_H