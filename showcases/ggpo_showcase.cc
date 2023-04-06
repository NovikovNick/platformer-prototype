#include <api.h>
#include <serializer.h>
#include <util.h>

#include <SFML/Graphics.hpp>

#include "chrono"
#include "showcase_callbacks.h"
#include "ui/info.h"
#include "ui/scene.h"
#include "util/util.h"

void handleKeyboardInput(sf::RenderWindow& window, Input& input,
                         platformer::ShowcaseCallback cb);

struct InputArgs {
  bool local;
  unsigned short local_port;
  char ip[32];
  unsigned short remote_port;
};

InputArgs validateAndParseInput(int argc, char* argv[]);

int main(int argc, char* argv[]) {
  const static sf::Color kBGColor(35, 35, 35);
  const static sf::Color kFstColor(255, 100, 0);
  const static sf::Color kSndColor(255, 17, 17);
  const static sf::Color kTrdColor(255, 255, 255);

  auto window = openWindow("Async showcase");
  window.setFramerateLimit(60);

  // 1. allocate memory
  uint8_t buf[512];

  auto arg = validateAndParseInput(argc, argv);
  window.setPosition({arg.local ? 0 : 1000, 0});

  // 2.1 Register remote host + port and pass your id (started from 0) and port
  RegisterPeer(arg.local_port, arg.local, arg.ip, arg.remote_port);

  // 2.2 Start game loop
  StartGame();

  // 3 Define callback to restart game
  platformer::ShowcaseCallback cb;
  cb.on_restart_handler = []() {
    std::thread([] {
      using namespace std::chrono_literals;
      // stop previus game loop
      StopGame();
      // wait until GetStatus() return STOPED
      if (wait(32ms, [] { return GetStatus() != GameStatus::STOPED; })) {
        // start new game loop
        StartGame();
      } else {
        platformer::debug("Unable to stop game loop. Terminate");
        exit(0);
      }
    }).detach();
  };

  // simple visualisation with rectangles
  platformer::Scene scena(kFstColor, kSndColor, kTrdColor);

  // text in the top left corner in the screen
  platformer::Info info = initInfo(kFstColor);
  const int state_key = info.addFormat("State: {:7s}#{:3d}\n");
  const int position_key = info.addFormat("Position[{:4d},{:4d}]\n");
  const int velocity_key = info.addFormat("Velocity[{:4d},{:4d}]\n");
  const int tick_index = info.addFormat("Tick:{:5d}\n");
  const int ser_index = info.addFormat("Serialized {:3d} bytes\n");
  const int status_index = info.addFormat("Status: {}\n");
  info.update(info.addFormat("Press 'R' to restart\n"));

  ser::GameState gs;
  Input input{false, false, false, false, false};
  int tick = 0;
  while (window.isOpen()) {
    info.update(status_index, toString(GetStatus()));
    info.update(tick_index, ++tick);

    handleKeyboardInput(window, input, cb);

    // 5. update input
    Update(input);

    // 6. write game state to buffer
    int length = GetState(buf);
    info.update(ser_index, length);

    // 7. deserialize game state from buffer
    gs.ParseFromArray(buf, length);

    {  // log and update text in the top left corner
      auto& p = gs.players()[0];
      auto pos_x = p.obj().position().x();
      auto pos_y = p.obj().position().y();
      auto vel_x = p.obj().velocity().x();
      auto vel_y = p.obj().velocity().y();

      info.update(state_key, toString(p.state()), p.state_frame());
      info.update(position_key, pos_x, pos_y);
      info.update(velocity_key, vel_x, vel_y);
    }

    // 6. render
    scena.update(gs);
    window.clear(kBGColor);
    window.draw(scena);
    window.draw(info);
    window.display();
  }

  // 7. release
  StopGame();

  return 0;
}

void handleKeyboardInput(sf::RenderWindow& window, Input& input,
                         platformer::ShowcaseCallback cb) {
  sf::Event event;
  while (window.pollEvent(event)) {
    switch (event.type) {
      case sf::Event::Closed:
        window.close();
        break;
      case sf::Event::KeyPressed:
        switch (event.key.code) {
          case sf::Keyboard::R:
            cb.on_restart_handler();
            break;
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
