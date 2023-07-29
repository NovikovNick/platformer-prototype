#include "ui/vector_product_visualizer.h"

#include "util.h"

namespace {
const static float kPi = std::acos(-1.0);
const static float kDegree = 180 / kPi;
const static float kPointRadius = 5.f;
const static float kSelectedRadius = 10.f;
const static Eigen::Vector2f kPointOffset(kPointRadius, kPointRadius);
const static Eigen::Vector2f kWorldOrigin(450, 450);
const static Eigen::Translation2f kWorldT(kWorldOrigin);
const static Eigen::Translation2f kWorldTInverted(kWorldT.inverse());

const static Eigen::Rotation2Df kTurn45(-kPi / 4);
const static Eigen::Rotation2Df kTurn135(-3 * kPi / 4);
const static Eigen::Rotation2Df kTurn225(-5 * kPi / 4);
const static Eigen::Rotation2Df kTurn315(-7 * kPi / 4);

sf::VertexArray initLine(const sf::Color color) {
  sf::VertexArray line(sf::LinesStrip, 2);
  line[0].color = line[1].color = color;
  return line;
}

sf::Text initLabel(const sf::Font& font,
                   const sf::Color color,
                   const std::string& text,
                   const int font_size) {
  sf::Text label(text, font, font_size);
  label.setColor(color);
  return label;
}

sf::CircleShape initPoint(const sf::Color color) {
  sf::CircleShape point(kPointRadius);
  point.setFillColor(color);
  point.setPosition({0, 0});
  return point;
}

sf::CircleShape initSelected(const sf::Color color) {
  sf::CircleShape point(kSelectedRadius);
  point.setFillColor(color);
  return point;
}

sf::CircleShape initUnitCircle(const sf::Color bg_color, const sf::Color color) {
  sf::CircleShape unit_circle(0.0f);
  unit_circle.setPointCount(75);
  unit_circle.setFillColor(bg_color);
  unit_circle.setOutlineThickness(1);
  unit_circle.setOutlineColor(color);
  return unit_circle;
}

sf::VertexArray initAngleArc(const sf::Color color) {
  sf::VertexArray angle_arc(sf::LinesStrip, 11);
  for (int i = 0; i < 11; ++i) angle_arc[i].color = color;
  return angle_arc;
}

inline Eigen::Vector2f toWorld(const Eigen::Vector2f& screen_coord) {
  return kWorldTInverted * screen_coord;
}

inline sf::Vector2f toScreen(const Eigen::Vector2f& world_coord) {
  Eigen::Vector2f screen_coord = kWorldT * world_coord;
  return sf::Vector2f(screen_coord.x(), screen_coord.y());
}

inline bool isHover(const Eigen::Vector2f& mouse, const Eigen::Vector2f& target) {
  const bool intersect_x = target.x() <= mouse.x() + kSelectedRadius &&
                           target.x() >= mouse.x() - kSelectedRadius;

  const bool intersect_y = target.y() <= mouse.y() + kSelectedRadius &&
                           target.y() >= mouse.y() - kSelectedRadius;

  return intersect_x && intersect_y;
}

}  // namespace

namespace platformer {

VectorProductVisualizer::VectorProductVisualizer(const sf::Font& font,
                                                 const sf::Color bg_color,
                                                 const sf::Color fst_color,
                                                 const sf::Color snd_color)
    : lhs_({0.f, 0.f}),
      rhs_({0.f, 0.f}),
      origin_({0.f, 0.f}),
      dot_(0.f),
      cross_(0.f),
      angle_rad_(0.f),
      cos_(0.f),
      sin_(0.f),
      lhs_length_(0.f),
      rhs_length_(0.f),
      lhs_label_(initLabel(font, fst_color, "LHS", 18)),
      rhs_label_(initLabel(font, snd_color, "RHS", 18)),
      quart1_label_(initLabel(font, snd_color, "  I  \n dot +\ncross +", 14)),
      quart2_label_(initLabel(font, snd_color, "  II \n dot -\ncross +", 14)),
      quart3_label_(initLabel(font, snd_color, "  III\n dot -\ncross -", 14)),
      quart4_label_(initLabel(font, snd_color, "  IV \n dot +\ncross -", 14)),
      lhs_vector_(VectorShape(fst_color)),
      rhs_vector_(VectorShape(snd_color)),
      cos_projection_line_(initLine(fst_color)),
      sin_projection_line_(initLine(fst_color)),
      rhs_perpendicular_line_(initLine(snd_color)),
      rhs_inverted_line_(initLine(snd_color)),
      origin_point_(initPoint(fst_color)),
      selected_(initSelected(fst_color)),
      unit_circle_(initUnitCircle(bg_color, snd_color)),
      angle_arc_(initAngleArc(fst_color)),
      drag_(false),
      lhs_selected_(false),
      rhs_selected_(false),
      origin_selected_(false){};

void VectorProductVisualizer::update(const Eigen::Vector2f mouse_position,
                                     const bool lft_mouse_pressed) {
  drag_ = lft_mouse_pressed;
  update(mouse_position);
};

void VectorProductVisualizer::update(const Eigen::Vector2f mouse_position) {
  auto mouse = toWorld(mouse_position);
  Eigen::Vector2f selected_offset(kSelectedRadius, kSelectedRadius);

  if (drag_) {
    if (lhs_selected_) {
      update(mouse, rhs_, origin_);
      selected_.setPosition(toScreen(lhs_ - selected_offset));
    } else if (rhs_selected_) {
      update(lhs_, mouse, origin_);
      selected_.setPosition(toScreen(rhs_ - selected_offset));
    } else if (origin_selected_) {
      update(lhs_, rhs_, mouse);
      selected_.setPosition(toScreen(origin_ - selected_offset));
    }
  } else {
    if (lhs_selected_ = isHover(mouse, lhs_)) {
      selected_.setPosition(toScreen(lhs_ - selected_offset));
    }
    if (rhs_selected_ = isHover(mouse, rhs_)) {
      selected_.setPosition(toScreen(rhs_ - selected_offset));
    }
    if (origin_selected_ = isHover(mouse, origin_)) {
      selected_.setPosition(toScreen(origin_ - selected_offset));
    }
  }
};

void VectorProductVisualizer::update(Eigen::Vector2f lhs,
                                     Eigen::Vector2f rhs,
                                     const Eigen::Vector2f origin) {
  lhs -= origin;
  rhs -= origin;
  lhs_length_ = lhs.norm();
  rhs_length_ = rhs.norm();
  lhs.normalize();
  rhs.normalize();
  dot_ = lhs.dot(rhs);
  cross_ = lhs.x() * rhs.y() - rhs.x() * lhs.y();  // lhs.cross(rhs);
  angle_rad_ = std::acos(dot_);
  cos_ = dot_;
  sin_ = std::sin(angle_rad_);

  auto lhs_cos = dot_ * lhs_length_;  // i.e. cos(angle) * length

  cos_projection_line_[0].position = toScreen(origin);
  cos_projection_line_[1].position = toScreen(rhs * lhs_cos + origin);
  sin_projection_line_[0].position = toScreen(lhs * lhs_length_ + origin);
  sin_projection_line_[1].position = toScreen(rhs * lhs_cos + origin);

  const Eigen::Vector2f d90{-rhs.y(), rhs.x()};   // turn to 90 degree
  const Eigen::Vector2f d270{rhs.y(), -rhs.x()};  // turn to 270 degree
  rhs_perpendicular_line_[0].position = toScreen(d90 * rhs_length_ + origin);
  rhs_perpendicular_line_[1].position = toScreen(d270 * rhs_length_ + origin);

  rhs_inverted_line_[0].position = toScreen(origin);
  rhs_inverted_line_[1].position = toScreen(rhs * -rhs_length_ + origin);

  const Eigen::Vector2f unit_circle_offset{rhs_length_, rhs_length_};
  unit_circle_.setRadius(rhs_length_);
  unit_circle_.setPosition(toScreen(origin - unit_circle_offset));

  const Eigen::Vector2f title_offset = origin - Eigen::Vector2f{15, 9};
  lhs_label_.setPosition(toScreen(lhs * (lhs_length_ + 30) + title_offset));
  rhs_label_.setPosition(toScreen(rhs * (rhs_length_ + 30) + title_offset));

  const Eigen::Vector2f quart_offset = origin - Eigen::Vector2f{20.f, 25.f};
  const Eigen::Vector2f quart1(kTurn45 * rhs * rhs_length_ / 2 + quart_offset);
  const Eigen::Vector2f quart2(kTurn135 * rhs * rhs_length_ / 2 + quart_offset);
  const Eigen::Vector2f quart3(kTurn225 * rhs * rhs_length_ / 2 + quart_offset);
  const Eigen::Vector2f quart4(kTurn315 * rhs * rhs_length_ / 2 + quart_offset);
  quart1_label_.setPosition(toScreen(quart1));
  quart2_label_.setPosition(toScreen(quart2));
  quart3_label_.setPosition(toScreen(quart3));
  quart4_label_.setPosition(toScreen(quart4));

  for (float i = 0; i < 11; ++i) {
    Eigen::Rotation2D rotation(angle_rad_ * (cross_ > 0 ? -0.1f : 0.1f) * i);
    angle_arc_[i].position = toScreen(rotation * rhs * 30 + origin);
  }

  lhs_ = lhs * lhs_length_ + origin;
  rhs_ = rhs * rhs_length_ + origin;
  origin_ = origin;

  lhs_vector_.update(kWorldT * origin, kWorldT * lhs_);
  rhs_vector_.update(kWorldT * origin, kWorldT * rhs_);
  origin_point_.setPosition(toScreen(origin_ - kPointOffset));
}

void VectorProductVisualizer::draw(sf::RenderTarget& target,
                                   sf::RenderStates states) const {
  if (drag_) target.draw(unit_circle_, states);

  if (lhs_selected_ || rhs_selected_ || origin_selected_)
    target.draw(selected_, states);

  target.draw(angle_arc_, states);
  target.draw(lhs_vector_, states);
  target.draw(rhs_vector_, states);

  if (drag_) target.draw(rhs_perpendicular_line_, states);
  if (drag_) target.draw(rhs_inverted_line_, states);
  if (drag_) target.draw(cos_projection_line_, states);
  if (drag_) target.draw(sin_projection_line_, states);
  if (drag_) target.draw(quart1_label_, states);
  if (drag_) target.draw(quart2_label_, states);
  if (drag_) target.draw(quart3_label_, states);
  if (drag_) target.draw(quart4_label_, states);

  if (!drag_) target.draw(lhs_label_, states);
  if (!drag_) target.draw(rhs_label_, states);

  target.draw(origin_point_, states);
};

inline float VectorProductVisualizer::dot() const { return dot_; }
inline float VectorProductVisualizer::cross() const { return cross_; }
inline float VectorProductVisualizer::angleDegree() const {
  return angle_rad_ * kDegree;
}
inline float VectorProductVisualizer::cos() const { return cos_; }
inline float VectorProductVisualizer::sin() const { return sin_; }
inline float VectorProductVisualizer::lhsAtan2() const {
  return std::atan2(lhs_.y() - origin_.y(), lhs_.x() - origin_.x()) * kDegree;
};
inline float VectorProductVisualizer::rhsAtan2() const {
  return std::atan2(rhs_.y() - origin_.y(), rhs_.x() - origin_.x()) * kDegree;
};
inline float VectorProductVisualizer::lhsLength() const { return lhs_length_; }
inline float VectorProductVisualizer::rhsLength() const { return rhs_length_; }

};  // namespace platformer
