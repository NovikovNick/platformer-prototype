#include "scalable_grid.h"

const static sf::Color kGreyColor(50, 50, 50);
const static sf::Color kWhiteColor(100, 100, 100);

namespace math {

ScalableGrid::ScalableGrid(const int unit)
    : grid_(sf::VertexArray(sf::Lines, 116)), unit_(static_cast<float>(unit >> 1)) {
  init(grid_, unit_);
};

void ScalableGrid::update(const int unit) {
  unit_ += unit;
  init(grid_, unit_);
};

void ScalableGrid::draw(sf::RenderTarget& target,
                        sf::RenderStates states) const {
  target.draw(grid_, states);
}

void ScalableGrid::init(sf::VertexArray& grid, const float unit) {
  for (int i = 0; i <= 56; i += 2) {
    grid[i] = {{i * unit, 0}, i % 4 == 0 ? kWhiteColor : kGreyColor};
    grid[i + 1] = {{i * unit, unit * 56},
                   i % 4 == 0 ? kWhiteColor : kGreyColor};
  }

  for (int i = 2; i <= 56; i += 2) {
    grid[56 + i] = {{0, i * unit},
                    i % 4 == 0 ? kWhiteColor : kGreyColor};
    grid[57 + i] = {{unit * 56, i * unit},
                    i % 4 == 0 ? kWhiteColor : kGreyColor};
  }
};

};  // namespace math