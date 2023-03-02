#include "player_shape.h"

namespace math {
PlayerShape::PlayerShape(const sf::Color main_color,
                         const sf::Color velocity_color, const GameObject curr)
    : player_shape_(sf::VertexArray(sf::Quads, 4)),
      curr_(curr),
      prev_(curr),
      velocity_vector_(VectorShape{velocity_color}) {
  for (int i = 0; i < 4; ++i) player_shape_[i].color = main_color;
}
void PlayerShape::update(const GameObject curr) {
  prev_ = curr_;
  curr_ = curr;
};

void PlayerShape::update(const float delta) {
  for (int i = 0; i < 4; ++i) {
    auto prev_x = static_cast<float>(prev_[i].x());
    auto prev_y = static_cast<float>(prev_[i].y());
    auto curr_x = static_cast<float>(curr_[i].x());
    auto curr_y = static_cast<float>(curr_[i].y());

    player_shape_[i].position = {std::lerp(prev_x, curr_x, delta),
                                 std::lerp(prev_y, curr_y, delta)};
  }

  {  // velocity vector
    auto prev_pos = prev_.position;
    auto prev_vel = prev_.velocity * FIXED{10};

    auto curr_pos = curr_.position;
    auto curr_vel = curr_.velocity * FIXED{10};

    auto prev_pos_x = static_cast<float>(prev_pos.x());
    auto prev_pos_y = static_cast<float>(prev_pos.y());
    auto prev_vel_x = static_cast<float>(prev_vel.x());
    auto prev_vel_y = static_cast<float>(prev_vel.y());

    auto curr_pos_x = static_cast<float>(curr_pos.x());
    auto curr_pos_y = static_cast<float>(curr_pos.y());
    auto curr_vel_x = static_cast<float>(curr_vel.x());
    auto curr_vel_y = static_cast<float>(curr_vel.y());

    velocity_vector_.update(
        {std::lerp(prev_pos_x, curr_pos_x, delta),
         std::lerp(prev_pos_y, curr_pos_y, delta)},
        {std::lerp(prev_pos_x + prev_vel_x, curr_pos_x + curr_vel_x, delta),
         std::lerp(prev_pos_y + prev_vel_y, curr_pos_y + curr_vel_y, delta)});
  }
};

void PlayerShape::draw(sf::RenderTarget& target,
                       sf::RenderStates states) const {
  target.draw(player_shape_, states);
  target.draw(velocity_vector_, states);
};
};  // namespace math
