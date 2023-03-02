#ifndef GEOM_2D_MATH_INFO_H
#define GEOM_2D_MATH_INFO_H
#include <SFML/Graphics.hpp>
#include <iostream>

namespace math {

class Info : public sf::Drawable {
  sf::Font font_;
  sf::Color text_color;
  std::vector<std::string> formats_;
  std::vector<sf::Text> rows_;

 public:

  Info(const sf::Font& font, const sf::Color text_color);

  int addFormat(const std::string& format);

  template <typename... Args>
  void update(const int index, Args... args);

  void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
};      // namespace math
#endif  // GEOM_2D_MATH_INFO_H