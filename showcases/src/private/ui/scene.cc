#include "ui/scene.h"

namespace platformer {

Scene::Scene(sf::Color fst_color, sf::Color snd_color, sf::Color trd_color)
    : fst_color(fst_color),
      snd_color(snd_color),
      trd_color(trd_color),
      shapes_(std::vector<RectShape>()),
      grid_(ScalableGrid(16, 40)) {}

void Scene::init(const ShowcaseContext& ctx) {
  shapes_.clear();

  shapes_.emplace_back(
      ctx.active_player_id == 0 ? trd_color : fst_color, 0, 0, 0, 0);
  shapes_.emplace_back(
      ctx.active_player_id != 0 ? trd_color : fst_color, 0, 0, 0, 0);

  shapes_.emplace_back(
      snd_color, 0, 0, ctx.position_1st_player.x, ctx.position_1st_player.y);
  shapes_.emplace_back(
      snd_color, 0, 0, ctx.position_2nd_player.x, ctx.position_2nd_player.y);

  for (const auto& it : ctx.platforms) {
    shapes_.emplace_back(
        fst_color, it.width, it.height, it.position.x, it.position.y);
  }

  grid_.update(ctx.scale, ctx.screen_offset_x, ctx.screen_offset_y);
}

void Scene::update(const ser::GameState& gs, const ShowcaseContext& ctx) {
  prev_game_state = std::move(curr_game_state);
  curr_game_state = gs;
  pivot_x = ctx.screen_offset_x;
  pivot_y = ctx.screen_offset_y;
  scale = ctx.scale;

  grid_.update(ctx.scale, ctx.screen_offset_x, ctx.screen_offset_y);
}

void Scene::update(const float dx) {
  auto& gs = curr_game_state;
  if (prev_game_state.players_size() <= 0) return;

  int offset = 0;
  for (int i = 0; i < gs.players_size() + offset; ++i) {
    auto& prev_player = prev_game_state.players()[i - offset];
    auto& curr_player = curr_game_state.players()[i - offset];

    auto prev_player_x = prev_player.obj().position().x();
    auto prev_player_y = prev_player.obj().position().y();
    auto curr_player_x = curr_player.obj().position().x();
    auto curr_player_y = curr_player.obj().position().y();

    shapes_[i].update(
        std::lerp(prev_player_x, curr_player_x, dx) * scale + pivot_x,
        std::lerp(prev_player_y, curr_player_y, dx) * -scale + pivot_y);

    shapes_[i].updateSize(prev_player.obj().width() * scale,
                          prev_player.obj().height() * -scale);
    shapes_[i].update(1);

    shapes_[i].update(prev_player.on_damage() ? sf::Color::Red : sf::Color::White);

    if (prev_player.state() == ser::PlayerState::BLOCK)
      shapes_[i].update(sf::Color::Yellow);
    if (prev_player.state() == ser::PlayerState::SQUAT_BLOCK)
      shapes_[i].update(sf::Color::Yellow);
    if (prev_player.state() == ser::PlayerState::DEATH) {
      uint8_t red = 255 * std::cos(gs.frame() / 10);
      uint8_t green = 255 * std::sin(gs.frame() / 5);
      uint8_t blue = 255 * std::sin(gs.frame() / 15);
      shapes_[i].update(sf::Color{red, green, blue});
    }
  }
  offset += gs.players_size();

  for (int i = offset; i < gs.melee_attacks_size() + offset; ++i) {
    auto& attack = gs.melee_attacks()[i - offset];
    shapes_[i].update(attack.position().x() * scale + pivot_x,
                      attack.position().y() * -scale + pivot_y);
    shapes_[i].updateSize(attack.width() * scale, attack.height() * -scale);
    shapes_[i].update(1);
  }
  offset += gs.players_size();

  for (int i = offset; i < gs.platforms_size() + offset; ++i) {
    auto& platform = gs.platforms()[i - offset];
    shapes_[i].update(platform.position().x() * scale + pivot_x,
                      platform.position().y() * -scale + pivot_y);
    shapes_[i].updateSize(platform.width() * scale, platform.height() * -scale);
    shapes_[i].update(1);
  }
}

void Scene::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  target.draw(grid_, states);
  for (const auto& it : shapes_) target.draw(it, states);
};

}  // namespace platformer