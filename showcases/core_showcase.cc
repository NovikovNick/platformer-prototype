#include <util.h>

#include <Eigen/Dense>
#include <SFML/Graphics.hpp>
#include <bitset>
#include <format>
#include <fpm/fixed.hpp>
#include <iostream>

#include "game_loop.h"
#include "game_object.h"
#include "game_state.h"
#include "ui/info.h"
#include "ui/player_shape.h"
#include "ui/scalable_grid.h"
#include "ui/vector_product_visualizer.h"
#include "util.h"

using namespace std::chrono;

namespace {

const static sf::Color kBGColor(35, 35, 35);
const static sf::Color kFstColor(255, 100, 0);
const static sf::Color kSndColor(255, 17, 17);
const static sf::Color kTrdColor(255, 255, 255);

std::string toString(const platformer::PlayerState& state) {
  switch (state) {
    case platformer::PlayerState::IDLE:
      return "IDLE";
    case platformer::PlayerState::RUN:
      return "RUN";
    case platformer::PlayerState::JUMP:
      return "JUMP";
    case platformer::PlayerState::FALLING:
      return "FALLING";
    case platformer::PlayerState::LANDING:
      return "LANDING";
    case platformer::PlayerState::ATTACK_ON_GROUND:
      return "ATTACK";
    case platformer::PlayerState::DEATH:
      return "DEATH";
  }
}

}  // namespace

int main() {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  auto mode = sf::VideoMode(896, 896);
  auto style = sf::Style::Default;
  sf::RenderWindow window(mode, "Generic Platformer", style, settings);

  // reduce the framerate to minimize laptop overheat :(
  // window.setFramerateLimit(24);
  window.setFramerateLimit(60);

  sf::Font font;
  if (!font.loadFromFile("resources/default_fnt.otf")) {
    platformer::debug("Unable to load default_fnt.otf!\n");
    return 1;
  }

  platformer::Info info(font, kFstColor);
  const int fps_index = info.addFormat("FPS: {:.0f}\n");
  const int dx_index = info.addFormat("Delta(sec): {:.6f}\n");
  const int tick_index = info.addFormat("Tick#{:4d}({:2d})- {:4.2f}%\n");
  const int mouse_index = info.addFormat("Mouse[{:4d},{:4d}]\n");
  const int intersection_index = info.addFormat("Intersect[{:4d},{:4d}]\n");

  platformer::VectorProductVisualizer visualizer(font, kBGColor, kSndColor,
                                                 kTrdColor);
  visualizer.update({0, -128}, {64, 0}, {0, 0});

  platformer::ScalableGrid grid(32);

  auto t0 = steady_clock::now();
  auto t1 = steady_clock::now();
  float elapsed = 0, dx = 0;
  std::bitset<5> input_bitset(0);

  auto gs = std::make_shared<platformer::GameState>();
  auto tick = std::make_shared<std::atomic<int>>(0);
  auto tick_rate = std::make_shared<std::atomic<int>>(60);
  auto tick_ratio = std::make_shared<std::atomic<float>>(0);
  auto p0_input = std::make_shared<std::atomic<int>>(0);
  auto p1_input = std::make_shared<std::atomic<int>>(0);
  platformer::GameLoop game_loop(gs, tick, tick_rate, tick_ratio, p0_input,
                                 p1_input);
  std::thread(game_loop).detach();

  int prev_tick = tick->load();
  int curr_tick = prev_tick;

  platformer::PlayerShape p0{kTrdColor, kSndColor, gs->getPlayer(0)};
  platformer::PlayerShape p1{kTrdColor, kSndColor, gs->getPlayer(1)};
  float t = tick_ratio->load();  // requires for lerp
  bool fst_player_active = true;

  while (window.isOpen()) {
    t1 = steady_clock::now();
    dx = duration_cast<microseconds>(t1 - t0).count() / 1e6;
    elapsed += dx;
    t0 = t1;

    // info.update(fps_index, 1 / dx);
    // info.update(dx_index, dx);

    sf::Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
          window.close();
          break;
        case sf::Event::KeyPressed:
          switch (event.key.code) {
            case sf::Keyboard::A:
              input_bitset[kInputLeft] = true;
              break;
            case sf::Keyboard::D:
              input_bitset[kInputRight] = true;
              break;
            case sf::Keyboard::W:
              input_bitset[kInputUp] = true;
              break;
            case sf::Keyboard::S:
              input_bitset[kInputDown] = true;
              break;
          }
          break;
        case sf::Event::KeyReleased:
          switch (event.key.code) {
            case sf::Keyboard::A:
              input_bitset[kInputLeft] = false;
              break;
            case sf::Keyboard::D:
              input_bitset[kInputRight] = false;
              break;
            case sf::Keyboard::W:
              input_bitset[kInputUp] = false;
              break;
            case sf::Keyboard::S:
              input_bitset[kInputDown] = false;
              break;
          }
          break;
        case sf::Event::MouseButtonPressed:
          if (event.mouseButton.button == sf::Mouse::Left) {
            auto [_, x, y] = event.mouseButton;
            visualizer.update({x, y}, true);
            input_bitset[kInputLKM] = true;
            //fst_player_active = !fst_player_active;
          }
          break;
        case sf::Event::MouseButtonReleased:
          if (event.mouseButton.button == sf::Mouse::Left) {
            auto [_, x, y] = event.mouseButton;
            visualizer.update({x, y}, false);
            input_bitset[kInputLKM] = false;
          }
          break;
        case sf::Event::MouseMoved: {
          auto [x, y] = event.mouseMove;
          visualizer.update({x, y});
          // info.update(mouse_index, x, y);
          gs->getPlatforms()[1].position = {FIX(x - x % 32), FIX(y - y % 32)};
          break;
        }
        case sf::Event::MouseWheelMoved: {
          auto [delta, x, y] = event.mouseWheel;
          tick_rate->fetch_add(delta);
          // grid.update(delta);
          break;
        }
      }
    }
    
    if (fst_player_active) {
      p0_input->store(input_bitset.to_ulong());
    } else {
      p1_input->store(input_bitset.to_ulong());
    }

    curr_tick = tick->load();
    t = tick_ratio->load();
    if (prev_tick != curr_tick) {
      prev_tick = curr_tick;
      p0.update(gs->getPlayer(0));
      p1.update(gs->getPlayer(1));
    }
    p0.update(t);
    p1.update(t);

    platformer::GameState gs_copy = gs->getStateProjection();
    // info.update(tick_index, curr_tick, tick_rate->load(), t);

    platformer::debug("{:7s}#{:3d}: ", toString(gs_copy.players_[0].state),
                      gs_copy.players_[0].state_frame);
    platformer::debug("pos[{:8.3f},{:8.3f}] vel[{:8.3f},{:8.3f}]\n",
                      static_cast<float>(gs_copy.players_[0].obj.position.x()),
                      static_cast<float>(gs_copy.players_[0].obj.position.y()),
                      static_cast<float>(gs_copy.players_[0].obj.velocity.x()),
                      static_cast<float>(gs_copy.players_[0].obj.velocity.y()));

    std::vector<sf::VertexArray> platform_shapes;
    for (const auto& platform : gs->getPlatforms()) {
      sf::VertexArray platform_shape(sf::Quads, 4);
      for (int i = 0; i < platform.size(); ++i) {
        platform_shape[i].position.x = static_cast<float>(platform[i].x());
        platform_shape[i].position.y = static_cast<float>(platform[i].y());
        platform_shape[i].color = kFstColor;
      }
      platform_shapes.push_back(platform_shape);
    }

    std::vector<sf::VertexArray> melee_shapes;
    for (const auto& melee_atack : gs_copy.melee_attack) {
      sf::VertexArray platform_shape(sf::Quads, 4);
      for (int i = 0; i < melee_atack.size(); ++i) {
        platform_shape[i].position.x = static_cast<float>(melee_atack[i].x());
        platform_shape[i].position.y = static_cast<float>(melee_atack[i].y());
        platform_shape[i].color = kSndColor;
      }
      melee_shapes.push_back(platform_shape);
    }

    window.clear(kBGColor);
    window.draw(grid);
    //window.draw(visualizer);
    window.draw(info);
    for (const auto& it : platform_shapes) window.draw(it);
    window.draw(p0);
    window.draw(p1);
    for (const auto& it : melee_shapes) window.draw(it);
    window.display();
  }
  return 0;
}
