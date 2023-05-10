#include "util.h"

#include <util.h>

platformer::Info initInfo(sf::Color color) {
  sf::Font font;
  if (!font.loadFromFile("resources/default_fnt.otf")) {
    throw std::runtime_error("Unable to load default_fnt.otf!\n");
  }
  return platformer::Info(font, color);
}

sf::RenderWindow openWindow(const std::string title) {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  auto mode = sf::VideoMode(896, 896);
  auto style = sf::Style::Default;
  return sf::RenderWindow(mode, title, style, settings);
};

std::string toString(const ser::PlayerState& state) {
  switch (state) {
    case ser::PlayerState::IDLE: return "IDLE";
    case ser::PlayerState::RUN: return "RUN";
    case ser::PlayerState::JUMP: return "JUMP";
    case ser::PlayerState::FALLING: return "FALLING";
    case ser::PlayerState::LANDING: return "LANDING";
    case ser::PlayerState::SQUAT: return "SQUAT";
    case ser::PlayerState::BLOCK: return "BLOCK";
    case ser::PlayerState::SQUAT_BLOCK: return "SQUAT_BLOCK";
    case ser::PlayerState::LOW_ATTACK: return "LOW_ATTACK";
    case ser::PlayerState::MID_ATTACK: return "MID_ATTACK";
    case ser::PlayerState::OVERHEAD_ATTACK: return "OVERHEAD_ATTACK";
    case ser::PlayerState::HIT_STUN: return "HIT_STUN";
    case ser::PlayerState::BLOCK_STUN: return "BLOCK_STUN";
    case ser::PlayerState::SQUAT_BLOCK_STUN: return "BLOCK_STUN";
    case ser::PlayerState::DEATH: return "DEATH";
  }
}

std::string toString(const ser::AttackPhase& phase) {
  switch (phase) {
    case ser::AttackPhase::STARTUP: return "STARTUP";
    case ser::AttackPhase::ACTIVE: return "ACTIVE";
    case ser::AttackPhase::RECOVERY: return "RECOVERY";
    case ser::AttackPhase::NONE: return "NONE";
  }
}

std::string toString(GameStatus status) {
  switch (status) {
    case GameStatus::RUN: return "Running";
    case GameStatus::STOPED: return "Stopped";
    case GameStatus::SYNC: return "Synchronizing";
    default: return "Unknown status";
  }
}

bool wait(const std::chrono::duration<long long, std::milli> duration,
          std::function<bool()> predicate) {
  using namespace std::chrono_literals;
  int wait_for_ms = duration.count();
  while (--wait_for_ms >= 0 && predicate()) {
    std::this_thread::sleep_for(1ms);
  }
  return wait_for_ms > 0;
};
