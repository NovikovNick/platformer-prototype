﻿#include <api.h>
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

void handleKeyboardInput(sf::RenderWindow& window,
                         Input& input,
                         platformer::ShowcaseCallback& cb,
                         platformer::ShowcaseHUD& hud,
                         platformer::ShowcaseContext& ctx);

int main(int argc, char* argv[]) {
  const static sf::Color kBGColor(35, 35, 35);
  const static sf::Color kFstColor(255, 100, 0);
  const static sf::Color kSndColor(255, 17, 17);
  const static sf::Color kTrdColor(255, 255, 255);

  auto window = openWindow("Platformer prototype", 960, 960);
  window.setFramerateLimit(60);
  window.setPosition(
      sf::Vector2i(sf::VideoMode::getDesktopMode().width / 2 - 960 / 2, 0));

  platformer::ShowcaseContext ctx;
  ctx.screen_offset_y = 960;
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
      if (!wait(100ms, [] { return GetStatus() != GameStatus::STOPED; })) {
        platformer::debug("Unable to stop game loop. Terminate");
        exit(0);
      }

      // init game state
      SetLocation(ctx.getLocation());

      scena.init(ctx);

      // register remote enpoint and tick rate
      Init({ctx.tick_rate, ctx.getRemoteEndpoint()});

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
  const int p1_health_index = info.addFormat("PLAYER 1 HEALTH: {} / {}\n");
  const int p2_health_index = info.addFormat("PLAYER 2 HEALTH: {} / {}\n");

  ser::GameState gs;
  Input input{false, false, false, false, false, false};
  int length;

  // lerp
  using namespace std::chrono;
  auto t0 = steady_clock::now();
  auto t1 = t0;
  float dx = 0;
  int prev_frame = 0;

  while (window.isOpen()) {
    info.update(status_index, toString(GetStatus()));
    info.update(tick_index, gs.frame());

    handleKeyboardInput(window, input, cb, hud, ctx);

    // update input
    Update(input);

    // write game state to buffer
    GetState(ctx.game_state_buf, &length);

    t1 = steady_clock::now();
    bool is_new_tick = prev_frame != gs.frame();
    if (is_new_tick) {
      dx = 0;
      prev_frame = gs.frame();
      scena.update(gs, ctx);
    } else {
      dx += duration_cast<microseconds>(t1 - t0).count() /
            static_cast<float>(getMicrosecondsInOneTick());
    }
    t0 = t1;
    if (ctx.log_dx) platformer::debug("frame = {:5d}, dx = {}\n", prev_frame, dx);

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
      info.update(p1_health_index,
                  gs.players()[0].current_health(),
                  gs.players()[0].max_health());
      info.update(p2_health_index,
                  gs.players()[1].current_health(),
                  gs.players()[1].max_health());

      if (ctx.log_player_state && is_new_tick) {
        platformer::debug("{:15s}#{:8s}#{:3d}: ",
                          toString(p.state()),
                          toString(p.attack_phase()),
                          p.state_frame());
        platformer::debug("pos[{:4d},{:4d}], ", pos_x, pos_y);
        platformer::debug("{:8s}, ", p.on_damage() ? "damage" : "");
        platformer::debug("vel[{:4d},{:4d}]\n", vel_x, vel_y);
      }
    }

    // render
    scena.update(dx);
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

void handleKeyboardInput(sf::RenderWindow& window,
                         Input& input,
                         platformer::ShowcaseCallback& cb,
                         platformer::ShowcaseHUD& hud,
                         platformer::ShowcaseContext& ctx) {
  sf::Event event;
  while (window.pollEvent(event)) {
    hud.handleEvent(window, event);
    switch (event.type) {
      case sf::Event::Closed: window.close(); break;
      case sf::Event::KeyPressed:
        switch (event.key.code) {
          case sf::Keyboard::A: input.leftPressed = true; break;
          case sf::Keyboard::D: input.rightPressed = true; break;
          case sf::Keyboard::W: input.upPressed = true; break;
          case sf::Keyboard::S: input.downPressed = true; break;
          case sf::Keyboard::LControl: ctx.ctrl_pressed = true; break;
        }
        break;
      case sf::Event::KeyReleased:
        switch (event.key.code) {
          case sf::Keyboard::A: input.leftPressed = false; break;
          case sf::Keyboard::D: input.rightPressed = false; break;
          case sf::Keyboard::W: input.upPressed = false; break;
          case sf::Keyboard::S: input.downPressed = false; break;
          case sf::Keyboard::LControl: ctx.ctrl_pressed = false; break;
        }
        break;
      case sf::Event::MouseButtonPressed:
        if (event.mouseButton.button == sf::Mouse::Left)
          input.leftMouseClicked = true;
        if (event.mouseButton.button == sf::Mouse::Right) {
          input.rightMouseClicked = true;
          ctx.right_mouse_pressed = true;
        }

        break;
      case sf::Event::MouseButtonReleased:
        if (event.mouseButton.button == sf::Mouse::Left) {
          input.leftMouseClicked = false;
        }
        if (event.mouseButton.button == sf::Mouse::Right) {
          input.rightMouseClicked = false;
          ctx.right_mouse_pressed = false;
        }
        break;

      case sf::Event::MouseMoved:
        if (ctx.right_mouse_pressed) {
          ctx.screen_offset_x -= ctx.prev_mouse_x - event.mouseMove.x;
          ctx.screen_offset_y -= ctx.prev_mouse_y - event.mouseMove.y;
        }
        ctx.prev_mouse_x = event.mouseMove.x;
        ctx.prev_mouse_y = event.mouseMove.y;
        break;

      case sf::Event::MouseWheelScrolled:
        ctx.scale += event.mouseWheelScroll.delta > 0 ? 0.125 : -0.125;
        break;
    }
  }
};