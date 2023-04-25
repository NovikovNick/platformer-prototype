#include <api.h>
#include <serializer.h>
#include <util.h>

#include <SFML/Graphics.hpp>
#include <chrono>

#include "showcase_callbacks.h"
#include "showcase_context.h"
#include "ui/info.h"
#include "ui/scene.h"
#include "ui/showcase_hud.h"
#include "util/util.h"

void handleKeyboardInput(sf::RenderWindow& window, Input& input,
                         platformer::ShowcaseCallback cb,
                         platformer::ShowcaseHUD& hud);

int main(int argc, char* argv[]) {
  const static sf::Color kBGColor(35, 35, 35);
  const static sf::Color kFstColor(255, 100, 0);
  const static sf::Color kSndColor(255, 17, 17);
  const static sf::Color kTrdColor(255, 255, 255);

  auto window = openWindow("Purgatorium");
  window.setFramerateLimit(60);

  platformer::ShowcaseContext ctx;
  platformer::ShowcaseHUD hud(window);

  // Request a public IP from the STUN server
  Endpoint endpoint = GetPublicEndpoint(7000);
  ctx.setLocalPublicEndpoint(endpoint);

  // simple visualisation with rectangles
  platformer::Scene scena(kFstColor, kSndColor, kTrdColor);
  scena.init(ctx);

  // Define callback to restart game
  platformer::ShowcaseCallback cb;
  cb.on_restart_handler = [&ctx, &scena]() {
    std::thread([&ctx, &scena] {
      // stop previus game loop
      StopGame();

      // wait until GetStatus() return STOPED
      using namespace std::chrono_literals;
      if (!wait(32ms, [] { return GetStatus() != GameStatus::STOPED; })) {
        platformer::debug("Unable to stop game loop. Terminate");
        exit(0);
      }

      // init game state
      Init(ctx.getLocation());

      scena.init(ctx);

      // register remote enpoint
      RegisterPeer(ctx.getRemoteEndpoint());

      // start new game loop
      StartGame();
    }).detach();
  };

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

  while (window.isOpen()) {
    info.update(status_index, toString(GetStatus()));
    info.update(tick_index, gs.frame());

    handleKeyboardInput(window, input, cb, hud);

    // update input
    Update(input);

    // write game state to buffer
    int length = GetState(ctx.game_state_buf);
    info.update(ser_index, length);

    // deserialize game state from buffer
    gs.ParseFromArray(ctx.game_state_buf, length);

    {  // log and update text in the top left corner
      auto& p = gs.players()[0];
      auto pos_x = p.obj().position().x();
      auto pos_y = p.obj().position().y();
      auto vel_x = p.obj().velocity().x();
      auto vel_y = p.obj().velocity().y();

      info.update(state_key, toString(p.state()), p.state_frame());
      info.update(position_key, pos_x, pos_y);
      info.update(velocity_key, vel_x, vel_y);

      /*
      platformer::debug("{:7s}#{:3d}: ", toString(p.state()), p.state_frame());
      platformer::debug("pos[{:4d},{:4d}], ", pos_x, pos_y);
      platformer::debug("vel[{:4d},{:4d}]\n", vel_x, vel_y);
      */
    }

    // render
    scena.update(gs);
    window.clear(kBGColor);
    window.draw(scena);
    window.draw(info);
    hud.draw(window, ctx, cb);
    window.display();
  }

  // release
  StopGame();

  return 0;
}

void handleKeyboardInput(sf::RenderWindow& window, Input& input,
                         platformer::ShowcaseCallback cb,
                         platformer::ShowcaseHUD& hud) {
  sf::Event event;
  while (window.pollEvent(event)) {
    hud.handleEvent(window, event);

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
