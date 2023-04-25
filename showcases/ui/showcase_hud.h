#ifndef PLATFORMER_UI_SHOWCASE_HUD_H
#define PLATFORMER_UI_SHOWCASE_HUD_H
#include <api.h>

#include <SFML/Graphics.hpp>
#include <vector>

#include "../showcase_callbacks.h"
#include "../showcase_context.h"

namespace platformer {

class ShowcaseHUD {
  std::vector<std::string> player_select_labels_;
  int selected_player_id_;
  sf::Clock deltaClock;
 public:
  ShowcaseHUD(sf::RenderWindow& window);
  ~ShowcaseHUD();

  void handleEvent(sf::RenderWindow& window, sf::Event& event) const;

  void draw(sf::RenderWindow& window, ShowcaseContext& ctx,
            ShowcaseCallback& cb);
};

};      // namespace platformer
#endif  // PLATFORMER_UI_SHOWCASE_HUD_H