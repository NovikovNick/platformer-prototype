#ifndef PLATFORMER_UI_RECT_SHAPE_H
#define PLATFORMER_UI_RECT_SHAPE_H
#include <Eigen/Dense>
#include <SFML/Graphics.hpp>
#include <vector>

namespace platformer {

class RectShape : public sf::Drawable {
  sf::VertexArray shape_;
  int width, height;
  float prev_x, prev_y, curr_x, curr_y;

 public:
  RectShape(const sf::Color color,
            const int width,
            const int height,
            const float x,
            const float y);
  void update(const float x, const float y);
  void updateSize(const int width, const int height);
  void update(const sf::Color color);
  void update(const float delta);
  void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
};      // namespace platformer
#endif  // PLATFORMER_UI_RECT_SHAPE_H