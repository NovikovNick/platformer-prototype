#include "info.h"

#include <format>

namespace math {

Info::Info(const sf::Font& font, const sf::Color text_color)
    : rows_(std::vector<sf::Text>{}),
      font_(font),
      text_color(text_color),
      formats_(std::vector<std::string>()){};

int Info::addFormat(const std::string& format) {
  formats_.push_back(format);
  int index = rows_.size();
  rows_.push_back(sf::Text("", font_, 18));
  rows_[index].setFillColor(text_color);
  rows_[index].setPosition(10, index * 20 + 10);
  return index;
}

template <typename... Args>
void Info::update(const int i, Args... args) {
  rows_[i].setString(std::vformat(formats_[i], std::make_format_args(args...)));
}

void Info::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  for (const auto& it : rows_) target.draw(it, states);
}

};  // namespace math