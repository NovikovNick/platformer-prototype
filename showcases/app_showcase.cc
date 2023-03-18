#include <serializer.h>
#include <util.h>

#include <Eigen/Dense>
#include <SFML/Graphics.hpp>
#include <bitset>
#include <format>
#include <fpm/fixed.hpp>
#include <iostream>

#include "app/input_args.h"
#include "app/net_game_loop.h"
#include "game_object.h"
#include "game_state.h"
#include "ui/info.h"
#include "ui/player_shape.h"
#include "ui/scalable_grid.h"
#include "ui/vector_product_visualizer.h"

using namespace std::chrono;
#define ARRAYSIZE(a)            \
  ((sizeof(a) / sizeof(*(a))) / \
   static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))
namespace {

const static sf::Color kBGColor(35, 35, 35);
const static sf::Color kFstColor(255, 100, 0);
const static sf::Color kSndColor(255, 17, 17);
const static sf::Color kTrdColor(255, 255, 255);

InputArgs validateAndParseInput(int argc, char* argv[]) {
  if (argc >= 4) {
    auto local_str = std::string(argv[1]);
    auto local_port_str = std::string(argv[2]);
    auto remode_addr_str = std::string(argv[3]);

    InputArgs res;
    res.local = local_str == "local";
    res.local_port = std::stoi(local_port_str);

    int i = 0;
    for (; remode_addr_str[i] != ':'; ++i) res.ip[i] = remode_addr_str[i];
    res.ip[i++] = '\0';
    std::string remote_port(remode_addr_str.begin() + i, remode_addr_str.end());
    res.remote_port = std::stoi(remote_port);

    platformer::debug("local peer : localhost:{}\n", res.local_port);
    platformer::debug("remote peer: {}:{}\n", res.ip, res.remote_port);
    return res;
  } else {
    platformer::debug("There are only {} args!\n", argc);
    for (int i = 0; i < argc; ++i) platformer::debug("arg: {}\n", argv[i]);
    return {};
  }
}
}  // namespace

int main(int argc, char* argv[]) {
  auto args = validateAndParseInput(argc, argv);

  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  auto mode = sf::VideoMode(896, 896);
  auto style = sf::Style::Default;
  sf::RenderWindow window(mode, "Generic Platformer", style, settings);
  bool fst_player_active;
  if (args.local) {
    window.setPosition({0, 0});
    fst_player_active = true;
  } else {
    window.setPosition({1000, 0});
    fst_player_active = false;
  }
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
  const int tick_index = info.addFormat("Tick#{:4d}\n");

  platformer::ScalableGrid grid(32);

  auto t0 = steady_clock::now();
  auto t1 = steady_clock::now();
  float elapsed = 0, dx = 0;
  std::bitset<5> input_bitset(0);

  auto gs = std::make_shared<platformer::GameState>();
  auto tick = std::make_shared<std::atomic<int>>(0);
  auto p0_input = std::make_shared<std::atomic<int>>(0);
  auto p1_input = std::make_shared<std::atomic<int>>(0);
  platformer::NetGameLoop game_loop(args, gs, tick, p0_input, p1_input);
  std::thread(game_loop).detach();

  int prev_tick = tick->load();
  int curr_tick = prev_tick;

  platformer::PlayerShape p0{kTrdColor, kSndColor, gs->getPlayer(0)};
  platformer::PlayerShape p1{kTrdColor, kSndColor, gs->getPlayer(1)};

  std::vector<sf::VertexArray> platform_shapes(gs->getPlatforms().size());
  std::vector<sf::VertexArray> melee_shapes(2);

  while (window.isOpen()) {
    t1 = steady_clock::now();
    dx = duration_cast<microseconds>(t1 - t0).count() / 1e6;
    elapsed += dx;
    t0 = t1;

    info.update(fps_index, 1 / dx);

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
            input_bitset[kInputLKM] = true;
          }
          break;
        case sf::Event::MouseButtonReleased:
          if (event.mouseButton.button == sf::Mouse::Left) {
            auto [_, x, y] = event.mouseButton;
            input_bitset[kInputLKM] = false;
          }
          break;
        case sf::Event::MouseMoved: {
          auto [x, y] = event.mouseMove;
          break;
        }
        case sf::Event::MouseWheelMoved: {
          auto [delta, x, y] = event.mouseWheel;
          break;
        }
      }
    }

    if (fst_player_active) {
      p0_input->store(input_bitset.to_ulong());
    } else {
      p1_input->store(input_bitset.to_ulong());
    }

    platformer::GameState gs_projection = gs->getStateProjection();
    curr_tick = tick->load();
    if (prev_tick != curr_tick) {
      prev_tick = curr_tick;
      p0.update(gs->getPlayer(0));
      p1.update(gs->getPlayer(1));
      p0.update(gs_projection.players_[0].on_damage ? kFstColor : kTrdColor);
      p1.update(gs_projection.players_[1].on_damage ? kFstColor : kTrdColor);
      p0.update(1);
      p1.update(1);

      {  // info + console
        auto& p = gs_projection.players_[0];
        auto pos_x = static_cast<float>(p.obj.position.x());
        auto pos_y = static_cast<float>(p.obj.position.y());
        auto vel_x = static_cast<float>(p.obj.velocity.x());
        auto vel_y = static_cast<float>(p.obj.velocity.y());

        info.update(tick_index, curr_tick);
      }
    } else {
      p0.update(1);
      p1.update(1);
    }

    for (int j = 0; const auto& platform : gs_projection.platforms_) {
      sf::VertexArray platform_shape(sf::Quads, 4);
      for (int i = 0; i < platform.size(); ++i) {
        platform_shape[i].position.x = static_cast<float>(platform[i].x());
        platform_shape[i].position.y = static_cast<float>(platform[i].y());
        platform_shape[i].color = kFstColor;
      }
      platform_shapes[j++] = platform_shape;
    }

    for (int j = 0; const auto& melee_atack : gs_projection.melee_attack) {
      sf::VertexArray platform_shape(sf::Quads, 4);
      for (int i = 0; i < melee_atack.size(); ++i) {
        platform_shape[i].position.x = static_cast<float>(melee_atack[i].x());
        platform_shape[i].position.y = static_cast<float>(melee_atack[i].y());
        platform_shape[i].color = kSndColor;
      }
      melee_shapes[j++] = platform_shape;
    }

    window.clear(kBGColor);
    window.draw(grid);
    window.draw(info);
    for (const auto& it : platform_shapes) window.draw(it);
    window.draw(p0);
    window.draw(p1);
    for (const auto& it : melee_shapes) window.draw(it);
    window.display();
  }
  return 0;
}
