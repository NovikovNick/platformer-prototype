#include "ui/scalable_grid.h"

const static sf::Color kGreyColor(50, 50, 50);
const static sf::Color kWhiteColor(100, 100, 100);

namespace platformer {

ScalableGrid::ScalableGrid(const int cell_size_px, const int cell_count)
    : grid_(sf::VertexArray(sf::Lines, cell_count * 4 + 2)),
      cell_size_px_(cell_size_px),
      cell_count_(cell_count),
      scale_(1.0),
      screen_offset_x_(0),
      screen_offset_y_(0) {
  updateGridCoordinate();
};

void ScalableGrid::update(const double scale,
                          const int screen_offset_x,
                          const int screen_offset_y) {
  scale_ = scale;
  screen_offset_x_ = screen_offset_x;
  screen_offset_y_ = screen_offset_y;

  updateGridCoordinate();
};

void ScalableGrid::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  target.draw(grid_, states);
}

void ScalableGrid::updateGridCoordinate() {
  const float unit = cell_size_px_ * scale_;
  sf::Color color;

  float col_top_y = screen_offset_y_;
  float col_bottom_y = -unit * cell_count_ * 2 + screen_offset_y_;
  float col_x = 0.0f;
  for (int i = 0; i <= cell_count_ * 2; i += 2) {
    color = i % 4 == 0 ? kWhiteColor : kGreyColor;
    col_x = i * unit + screen_offset_x_;

    grid_[i] = {{col_x, col_top_y}, color};
    grid_[i + 1] = {{col_x, col_bottom_y}, color};
  }

  float row_left_x = screen_offset_x_;
  float row_right_x = unit * cell_count_ * 2 + screen_offset_x_;
  float row_y = 0.0f;
  for (int i = 2; i <= cell_count_ * 2; i += 2) {
    color = i % 4 == 0 ? kWhiteColor : kGreyColor;
    row_y = i * -unit + screen_offset_y_;

    grid_[cell_count_ * 2 + i] = {{row_left_x, row_y}, color};
    grid_[cell_count_ * 2 + i + 1] = {{row_right_x, row_y}, color};
  }
};

};  // namespace platformer