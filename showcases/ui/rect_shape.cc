#include "rect_shape.h"

namespace platformer {
RectShape::RectShape(const sf::Color color, const int width, const int height,
                     const float x, const float y)
    : shape_(sf::VertexArray(sf::Quads, 4)),
      width(width),
      height(height),
      curr_x(x),
      curr_y(y),
      prev_x(x),
      prev_y(y) {
  shape_[0].position = {x + 0, y + 0};
  shape_[1].position = {x + width, y + 0};
  shape_[2].position = {x + width, y + height};
  shape_[3].position = {x + 0, y + height};

  for (int i = 0; i < 4; ++i) shape_[i].color = color;
}
void RectShape::update(const float x, const float y) {
  prev_x = curr_x;
  prev_y = curr_y;
  curr_x = x;
  curr_y = y;
}
void RectShape::updateSize(const int new_width, const int new_height) {
  width = new_width;
  height = new_height;
}

void RectShape::update(const sf::Color color) {
  for (int i = 0; i < 4; ++i) shape_[i].color = color;
};

void RectShape::update(const float delta) {
  auto x = std::lerp(prev_x, curr_x, delta);
  auto y = std::lerp(prev_y, curr_y, delta);

  shape_[0].position = {x + 0, y + 0};
  shape_[1].position = {x + width, y + 0};
  shape_[2].position = {x + width, y + height};
  shape_[3].position = {x + 0, y + height};
};

void RectShape::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  target.draw(shape_, states);
};
};  // namespace platformer
