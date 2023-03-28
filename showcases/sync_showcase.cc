#include <api.h>
#include <serializer.h>

#include <SFML/Graphics.hpp>
#include <bitset>
#include <format>
#include <iostream>
#include <numeric>

#include "ui/info.h"
#include "ui/rect_shape.h"
#include "ui/scalable_grid.h"

using namespace std::chrono;

const static sf::Color kBGColor(35, 35, 35);
const static sf::Color kFstColor(255, 100, 0);
const static sf::Color kSndColor(255, 17, 17);
const static sf::Color kTrdColor(255, 255, 255);

class Scene : public sf::Drawable {
  platformer::ScalableGrid grid_;
  std::vector<platformer::RectShape> shapes_;

 public:
  platformer::Info info;

  Scene();
  void update(const ser::GameState& gs);
  void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

sf::RenderWindow openWindow(const std::string title);

void handleKeyboardInput(sf::RenderWindow& window, Input& input);

int main() {
  auto window = openWindow("Frontend sync showcase");
  window.setFramerateLimit(60);

  // 1. allocate memory
  uint8_t buf[512];

  // 2. Init Game state
  StartGame();

  Scene scena;
  const int tick_index = scena.info.addFormat("Tick:{:5d}\n");
  const int ser_index = scena.info.addFormat("Serialized {:3d} bytes\n");

  ser::GameState gs;
  Input input{false, false, false, false, false};
  int tick = 0;
  while (window.isOpen()) {
    scena.info.update(tick_index, ++tick);

    handleKeyboardInput(window, input);

    // 3. update input
    Update(input);

    // 4. write game state to buffer
    int length = GetState(buf);
    scena.info.update(ser_index, length);

    // 5. deserialize game state from buffer
    gs.ParseFromArray(buf, length);

    // 6. render
    scena.update(gs);
    window.clear(kBGColor);
    window.draw(scena);
    window.display();
  }

  // 7. release
  StopGame();

  return 0;
}

platformer::Info initInfo() {
  sf::Font font;
  if (!font.loadFromFile("resources/default_fnt.otf")) {
    throw std::runtime_error("Unable to load default_fnt.otf!\n");
  }
  return platformer::Info(font, kFstColor);
}

Scene::Scene()
    : shapes_(std::vector<platformer::RectShape>()),
      grid_(platformer::ScalableGrid(32)),
      info(initInfo()) {}

void Scene::update(const ser::GameState& gs) {
  if (shapes_.empty()) {
    for (const auto& player : gs.players())
      shapes_.emplace_back(kTrdColor, player.obj().width(),
                           player.obj().height(), player.obj().position().x(),
                           player.obj().position().y());

    for (const auto& attack : gs.melee_attacks())
      shapes_.emplace_back(kSndColor, attack.width(), attack.height(),
                           attack.position().x(), attack.position().y());

    for (const auto& platform : gs.platforms())
      shapes_.emplace_back(kFstColor, platform.width(), platform.height(),
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
  target.draw(info, states);
  for (const auto& it : shapes_) target.draw(it, states);
};

sf::RenderWindow openWindow(const std::string title) {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  auto mode = sf::VideoMode(896, 896);
  auto style = sf::Style::Default;
  return sf::RenderWindow(mode, title, style, settings);
};

void handleKeyboardInput(sf::RenderWindow& window, Input& input) {
  sf::Event event;
  while (window.pollEvent(event)) {
    switch (event.type) {
      case sf::Event::Closed:
        window.close();
        break;
      case sf::Event::KeyPressed:
        switch (event.key.code) {
          case sf::Keyboard::A:
            input.leftPressed = true;
            break;
          case sf::Keyboard::D:
            input.rightPressed = true;
            break;
          case sf::Keyboard::W:
            input.upPressed = true;
            break;
          case sf::Keyboard::S:
            input.downPressed = true;
            break;
        }
        break;
      case sf::Event::KeyReleased:
        switch (event.key.code) {
          case sf::Keyboard::A:
            input.leftPressed = false;
            break;
          case sf::Keyboard::D:
            input.rightPressed = false;
            break;
          case sf::Keyboard::W:
            input.upPressed = false;
            break;
          case sf::Keyboard::S:
            input.downPressed = false;
            break;
        }
        break;
      case sf::Event::MouseButtonPressed:
        if (event.mouseButton.button == sf::Mouse::Left) {
          input.leftMouseClicked = true;
        }
        break;
      case sf::Event::MouseButtonReleased:
        if (event.mouseButton.button == sf::Mouse::Left) {
          input.leftMouseClicked = false;
        }
        break;
    }
  }
};