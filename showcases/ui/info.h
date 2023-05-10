#ifndef PLATFORMER_UI_INFO_H
#define PLATFORMER_UI_INFO_H
#include <SFML/Graphics.hpp>
#include <format>
#include <iostream>

namespace platformer {

class Info : public sf::Drawable {
  sf::Font font_;
  sf::Color text_color;
  std::vector<std::string> formats_;
  std::vector<sf::Text> rows_;

 public:
  Info(const sf::Font& font, const sf::Color text_color);

  int addFormat(const std::string& format);

  template <typename... Args>
  void update(const int i, Args... args) {
    rows_[i].setString(
        std::vformat(formats_[i], std::make_format_args(args...)));
  }

  void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
};      // namespace platformer
#endif  // PLATFORMER_UI_INFO_H