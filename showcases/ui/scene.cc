#include "scene.h"

namespace platformer {

Scene::Scene(sf::Color fst_color, sf::Color snd_color, sf::Color trd_color)
    : fst_color(fst_color),
      snd_color(snd_color),
      trd_color(trd_color),
      shapes_(std::vector<platformer::RectShape>()),
      grid_(platformer::ScalableGrid(32)) {}

void Scene::init(const ShowcaseContext ctx) {
  shapes_.clear();

  shapes_.emplace_back(ctx.active_player_id == 0 ? trd_color : fst_color, 0, 0,
                       0, 0);
  shapes_.emplace_back(ctx.active_player_id != 0 ? trd_color : fst_color, 0, 0,
                       0, 0);

  shapes_.emplace_back(snd_color, 0, 0, ctx.position_1st_player.x,
                       ctx.position_1st_player.y);
  shapes_.emplace_back(snd_color, 0, 0, ctx.position_2nd_player.x,
                       ctx.position_2nd_player.y);

  for (const auto& it : ctx.platforms) {
    shapes_.emplace_back(fst_color, it.width, it.height, it.position.x,
                         it.position.y);
  }
}

void Scene::update(const ser::GameState& gs) {
  int offset = 0;
  for (int i = 0; i < gs.players_size() + offset; ++i) {
    auto& player = gs.players()[i - offset];
    shapes_[i].update(player.obj().position().x(), player.obj().position().y());
    shapes_[i].updateSize(player.obj().width(), player.obj().height());
    shapes_[i].update(1);

    shapes_[i].update(player.on_damage() ? sf::Color::Red : sf::Color::White);

    if (player.state() == ser::PlayerState::BLOCK)
      shapes_[i].update(sf::Color::Yellow);
    if (player.state() == ser::PlayerState::SQUAT_BLOCK)
      shapes_[i].update(sf::Color::Yellow);
    if (player.state() == ser::PlayerState::DEATH) {
      uint8_t red = 255 * std::cos(gs.frame() / 10);
      uint8_t green = 255 * std::sin(gs.frame() / 5);
      uint8_t blue = 255 * std::sin(gs.frame() / 15);
      shapes_[i].update(sf::Color{red, green, blue});
    }
  }
  offset += gs.players_size();

  for (int i = offset; i < gs.melee_attacks_size() + offset; ++i) {
    auto& attack = gs.melee_attacks()[i - offset];
    shapes_[i].update(attack.position().x(), attack.position().y());
    shapes_[i].updateSize(attack.width(), attack.height());
    shapes_[i].update(1);
  }
  offset += gs.players_size();

  for (int i = offset; i < gs.platforms_size() + offset; ++i) {
    auto& platform = gs.platforms()[i - offset];
    shapes_[i].update(platform.position().x(), platform.position().y());
    shapes_[i].updateSize(platform.width(), platform.height());
    shapes_[i].update(1);
  }
}

void Scene::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  target.draw(grid_, states);
  for (const auto& it : shapes_) target.draw(it, states);
};

}  // namespace platformer