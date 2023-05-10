#include "game_state.h"

#include <collision_service.h>
#include <game_state_service.h>
#include <locomotion_fsm.h>
#include <util.h>

#include <bitset>

namespace platformer {

GameState::GameState()
    : frame(0),
      players_(std::vector<Player>{}),
      melee_attack(std::vector<GameObject>{}),
      platforms_(std::vector<GameObject>{}),
      left_top_mesh_(
          {{kZero, kOne}, {kOne, kOne}, {kOne, kZero}, {kZero, kZero}}),
      state_service_({}) {
  players_.emplace_back();
  players_.emplace_back();

  melee_attack.emplace_back(0, 0, left_top_mesh_);
  melee_attack.emplace_back(0, 0, left_top_mesh_);
}

GameState::GameState(GameState& src) {
  players_ = src.players_;
  platforms_ = src.platforms_;
  melee_attack = src.melee_attack;
  frame = src.frame;
}

// UPDATE METHOD

void GameState::update(const int p0_input, const int p1_input) {
  int player_count = 2;
  std::scoped_lock lock{mutex_};
  ++frame;

  for (int player_id = 0; player_id < player_count; ++player_id) {
    auto& player = players_[player_id];
    // 1. calculate forces
    calculateVelocity(player_id, player_id == 0 ? p0_input : p1_input);

    // 2. apply forces
    player.obj.position += player.obj.velocity;

    // 3. resolve collisions
    for (int enemy_id = 0; enemy_id < player_count; ++enemy_id) {
      if (enemy_id == player_id) continue;
      CollistionService::resolveCollision(player.obj, players_[enemy_id].obj);
    }
    for (const auto& it : platforms_)
      CollistionService::resolveCollision(player.obj, it);

    player.on_ground = checkPlatform(player_id);

    auto& vel_y = player.obj.velocity.y();
    if (player.on_ground && vel_y > kZero) vel_y = kZero;
  }

  // 4. resolve player direction
  auto& p1 = players_[0];
  auto& p2 = players_[1];
  p1.left_direction = p1.obj.position.x() > p2.obj.position.x();
  p2.left_direction = !(p1.left_direction);

  // 5. resolve player damage
  for (int id = 0; id < kPlayerCount; ++id) resolveDamage(id);
}

// SETUP LOCATION

void GameState::removeAllPlatforms() {
  std::scoped_lock lock{mutex_};
  platforms_.clear();
}

void GameState::addPlatform(const int width, const int height, const int x,
                            const int y) {
  std::scoped_lock lock{mutex_};
  auto& platform = platforms_.emplace_back(width, height, left_top_mesh_);
  platform.position = {FIX(x), FIX(y)};
}

void GameState::setPlayerPosition(const int id, const int x, const int y) {
  players_[id].obj.position = {FIX(x), FIX(y)};
}

GameState GameState::getStateProjection() {
  std::scoped_lock lock{mutex_};
  return *this;
};

// CONVINIENCE

GameObject GameState::getPlayer(const int player_id) {
  std::scoped_lock lock{mutex_};
  return players_[player_id].obj;
}

std::vector<GameObject>& GameState::getPlatforms() { return platforms_; }

std::unique_lock<std::mutex> GameState::lock() {
  return std::unique_lock{mutex_};
}

GameState& GameState::operator=(const GameState& src) {
  players_ = src.players_;
  platforms_ = src.platforms_;
  melee_attack = src.melee_attack;
  frame = src.frame;
  return *this;
};

bool GameState::checkPlatform(const int player_id) {
  auto& player = players_[player_id].obj;
  for (const auto& platform : platforms_) {
    auto [player_min_x, player_max_x] = player.getProjectionMinMax(0);
    auto [platform_min_x, platform_max_x] = platform.getProjectionMinMax(0);

    auto [__, player_max_y] = player.getProjectionMinMax(1);
    auto [platform_min_y, _] = platform.getProjectionMinMax(1);

    if (player_max_y == platform_min_y && platform_max_x > player_min_x &&
        player_max_x > platform_min_x) {
      return true;
    }
  }
  return false;
}

void GameState::resolveDamage(const int player_id) {
  auto& player = players_[player_id];
  auto state = player.state;

  for (int i = 0; i < kPlayerCount; ++i) {
    if (player_id == i) continue;
    auto& enemy = players_[i];

    if (state_service_.isAttack(enemy.state)) {
      auto [damage, chip_damage] = state_service_.getFrameData(enemy.state);
      auto [x, y] = CollistionService::isIntersect(melee_attack[i], player.obj);

      if (x != kZero && y != kZero) {
        if (state == PlayerState::BLOCK) {
          state = PlayerState::BLOCK_STUN;
          if (!player.on_damage) player.current_health -= chip_damage;

        } else if (state == PlayerState::SQUAT_BLOCK) {
          state = PlayerState::SQUAT_BLOCK_STUN;
          if (!player.on_damage) player.current_health -= chip_damage;

        } else {
          state = PlayerState::HIT_STUN;
          if (!player.on_damage) player.current_health -= damage;
        }
        player.on_damage = true;
      } else {
        player.on_damage = false;
      }

      if (player.current_health <= 0) state = PlayerState::DEATH;

    } else {
      player.on_damage = false;
    }

    player.updateState(state);
  }
}

void GameState::calculateVelocity(const int player_id, const int player_input) {
  auto& player = players_[player_id];
  auto& vel_x = player.obj.velocity.x();
  auto& vel_y = player.obj.velocity.y();
  auto prev_state = player.state;

  if (!player.on_ground) vel_y += FIX{kAccelerationGravity};

  if (prev_state == PlayerState::DEATH) {
    melee_attack[player_id].height_ = melee_attack[player_id].width_ = 0;
  } else {
    std::bitset<6> input(player_input);
    if (input[kInputLeft]) vel_x -= FIX{kAccelerationX};
    if (input[kInputRight]) vel_x += FIX{kAccelerationX};

    // 1. calculate player state and frame
    updatePlayerState(player_id, player_input);

    // 2. update input
    auto curr_state_data = state_service_.getStateData(player.state);
    auto prev_state_date = state_service_.getStateData(prev_state);
    if (!curr_state_data.moveable) vel_x = kZero;

    curr_state_data.on_state(player, melee_attack[player_id]);

    if (player.state != prev_state)
      prev_state_date.on_state_out(player, melee_attack[player_id]);

    if (curr_state_data.is_duck) {
      player.obj.height_ = 64;
    } else if (!curr_state_data.is_duck && prev_state_date.is_duck) {
      player.obj.height_ = 128;
      player.obj.position.y() -= 64;
    }

    if (!input[kInputLeft] && !input[kInputRight]) {
      vel_x *= player.on_ground ? FIX{0.5} : FIX{0.9};
      if (static_cast<int>(vel_x) == 0) vel_x = kZero;
    }
    vel_x = std::clamp(vel_x, FIX{-kMaxVelocityX}, FIX{kMaxVelocityX});
  }
  vel_y = std::clamp(vel_y, -kJumpDelta * kJump, FIX{kMaxVelocityFall});
};

void GameState::updatePlayerState(const int player_id, const int player_input) {
  auto& player = players_[player_id];

  auto fsm = PlayerSM{player};
  fsm.process_event(InputNone{});  // !
  fsm.process_event(InputNone{});

  std::bitset<6> prev_input(player.prev_input);
  std::bitset<6> input(player_input);
  ++player.state_frame;

  if (input[kInputDown]) fsm.process_event(InputDown{});
  if (input[kInputLeft]) fsm.process_event(InputLeft{});
  if (input[kInputRight]) fsm.process_event(InputRight{});
  if (input[kInputUp] && !prev_input[kInputUp]) fsm.process_event(InputUp{});

  if (input[kInputLKM] && !prev_input[kInputLKM]) {
    if (prev_input[kInputDown]) {
      fsm.process_event(InputDownLKM{});
    } else if (player.left_direction && prev_input[kInputRight]) {
      fsm.process_event(InputBackwardLKM{});
    } else if (!player.left_direction && prev_input[kInputLeft]) {
      fsm.process_event(InputBackwardLKM{});
    } else {
      fsm.process_event(InputLKM{});
    }
  }

  if (input[kInputRKM]) {
    if (prev_input[kInputDown]) {
      fsm.process_event(InputDownRKM{});
    } else {
      fsm.process_event(InputRKM{});
    }
  }

  player.prev_input = input.to_ulong();

  player.updateState(getState(fsm));
};
};  // namespace platformer
