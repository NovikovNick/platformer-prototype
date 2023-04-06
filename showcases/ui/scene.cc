#include "scene.h"

namespace platformer {

Scene::Scene(sf::Color fst_color, sf::Color snd_color, sf::Color trd_color)
    : fst_color(fst_color),
      snd_color(snd_color),
      trd_color(trd_color),
      shapes_(std::vector<platformer::RectShape>()),
      grid_(platformer::ScalableGrid(32)) {}

void Scene::update(const ser::GameState& gs) {
  if (shapes_.empty()) {
    for (const auto& player : gs.players())
      shapes_.emplace_back(trd_color, player.obj().width(),
                           player.obj().height(), player.obj().position().x(),
                           player.obj().position().y());

    for (const auto& attack : gs.melee_attacks())
      shapes_.emplace_back(snd_color, attack.width(), attack.height(),
                           attack.position().x(), attack.position().y());

    for (const auto& platform : gs.platforms())
      shapes_.emplace_back(fst_color, platform.width(), platform.height(),
                           platform.position().x(), platform.position().y());
  }

  for (int player_id = 0; player_id < gs.players_size(); ++player_id) {
    shapes_[player_id].update(gs.players()[player_id].obj().position().x(),
                              gs.players()[player_id].obj().position().y());
    shapes_[player_id].update(1);
  }

  for (int player_id = 0; player_id < gs.melee_attacks_size(); ++player_id) {
    shapes_[player_id + 2].update(gs.melee_attacks()[player_id].position().x(),
                                  gs.melee_attacks()[player_id].position().y());
    shapes_[player_id + 2].updateSize(gs.melee_attacks()[player_id].width(),
                                      gs.melee_attacks()[player_id].height());
    shapes_[player_id + 2].update(1);
  }
}

void Scene::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  target.draw(grid_, states);
  for (const auto& it : shapes_) target.draw(it, states);
};

}  // namespace platformer