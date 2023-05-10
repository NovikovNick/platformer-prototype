#ifndef PLATFORMER_UI_SCENE_H
#define PLATFORMER_UI_SCENE_H
#include <api.h>
#include <schema.pb.h>

#include <SFML/Graphics.hpp>
#include <vector>

#include "../showcase_context.h"
#include "info.h"
#include "rect_shape.h"
#include "scalable_grid.h"

namespace platformer {

class Scene : public sf::Drawable {
  ser::GameState prev_game_state, curr_game_state;
  ScalableGrid grid_;
  std::vector<platformer::RectShape> shapes_;
  int state_key, position_key, velocity_key;
  sf::Color fst_color, snd_color, trd_color;

 public:
  Scene(sf::Color fst_color, sf::Color snd_color, sf::Color trd_color);
  void init(const ShowcaseContext ctx);
  void update(const ser::GameState& gs);
  void update(const float dx);
  void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
};      // namespace platformer
#endif  // PLATFORMER_UI_SCENE_H