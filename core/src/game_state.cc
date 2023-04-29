#include "game_state.h"

#include <util.h>

#include <bitset>
#include <fpm/math.hpp>

#include "game_state.h"

namespace {

inline VECTOR_2 normal(const VECTOR_2& val) {
  if (val.x() == kZero && val.y() == kZero) return {kZero, kZero};
  return val / fpm::sqrt(fpm::pow(val.x(), 2) + fpm::pow(val.y(), 2));
}

std::pair<FIX, FIX> isIntersect(const platformer::GameObject& lhs,
                                const platformer::GameObject& rhs) {
  auto [lhs_min_x, lhs_max_x] = lhs.getProjectionMinMax(0);
  auto [rhs_min_x, rhs_max_x] = rhs.getProjectionMinMax(0);
  auto [lhs_min_y, lhs_max_y] = lhs.getProjectionMinMax(1);
  auto [rhs_min_y, rhs_max_y] = rhs.getProjectionMinMax(1);

  // rhs_max_y >= lhs_min_y && rhs_min_y <= lhs_max_y;
  if (rhs_max_x - lhs_min_x < kZero) return {kZero, kZero};
  if (lhs_max_x - rhs_min_x < kZero) return {kZero, kZero};
  if (rhs_max_y - lhs_min_y < kZero) return {kZero, kZero};
  if (lhs_max_y - rhs_min_y < kZero) return {kZero, kZero};

  FIX diff_x = std::min(rhs_max_x - lhs_min_x, lhs_max_x - rhs_min_x);
  FIX diff_y = std::min(rhs_max_y - lhs_min_y, lhs_max_y - rhs_min_y);

  if (rhs_max_x > lhs_max_x) diff_x *= -1;
  if (rhs_max_y > lhs_max_y) diff_y *= -1;
  return {diff_x, diff_y};
}
}  // namespace

namespace platformer {

GameState::GameState()
    : frame(0),
      players_(std::vector<Player>{}),
      melee_attack(std::vector<GameObject>{}),
      platforms_(std::vector<GameObject>{}),
      left_top_mesh_(
          {{kZero, kOne}, {kOne, kOne}, {kOne, kZero}, {kZero, kZero}}) {
  players_.emplace_back();
  players_.emplace_back();

  melee_attack.emplace_back(0, 0, left_top_mesh_);
  melee_attack.emplace_back(0, 0, left_top_mesh_);

  fsms_.emplace_back(players_[0]);
  fsms_.emplace_back(players_[1]);
}

GameState::GameState(GameState& src) {
  players_ = src.players_;
  platforms_ = src.platforms_;
  melee_attack = src.melee_attack;
  frame = src.frame;
}

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

void GameState::refreshStateMachine() {
  fsms_.clear();
  fsms_.emplace_back(players_[0]);
  fsms_.emplace_back(players_[1]);
}

void GameState::update(const int p0_input, const int p1_input,
                       const int frames) {
  int player_count = 2;
  std::scoped_lock lock{mutex_};
  ++frame;

  for (int player_id = 0; player_id < 2; ++player_id) {
    auto& player = players_[player_id];
    auto& fsm = fsms_[player_id];
    //fsm(player);

    auto& vel_x = player.obj.velocity.x();
    auto& vel_y = player.obj.velocity.y();

    std::bitset<6> input(player_id == 0 ? p0_input : p1_input);
    std::bitset<6> prev_input(player.prev_input);

    if (input[kInputLeft]) vel_x += FIX{-kAccelerationX};
    if (input[kInputRight]) vel_x += FIX{kAccelerationX};

    // 1. update FSM
    ++player.state_frame;

    fsm.process_event(InputNone{});
    if (input[kInputLeft]) {
      fsm.process_event(InputLeft{});
      player.left_direction = true;
    }
    if (input[kInputRight]) {
      fsm.process_event(InputRight{});
      player.left_direction = false;
    }
    if (input[kInputUp] && !prev_input[kInputUp]) fsm.process_event(InputUp{});
    if (input[kInputLKM] && !prev_input[kInputLKM])
      fsm.process_event(InputLKM{});

    if (input[kInputRKM] && !prev_input[kInputRKM])
      player.current_health = std::max(player.current_health - 1, 0);

    player.prev_input = input.to_ulong();

    player.updateState(getState(fsm));

    // 2. update input
    if (player.is(PlayerState::FALLING)) vel_y += FIX{kAccelerationGravity};
    if (player.is(PlayerState::ATTACK_ON_GROUND)) {
      melee_attack[player_id].position.x() =
          player.obj.position.x() + player.obj.width_ / 2;
      melee_attack[player_id].position.y() = player.obj.position.y();

      melee_attack[player_id].width_ =
          24 * player.state_frame * (player.left_direction ? -1 : 1);
      melee_attack[player_id].height_ = player.obj.height_;
    } else {
      melee_attack[player_id].width_ = 0;
      melee_attack[player_id].height_ = 0;
    }

    if (player.is(PlayerState::JUMP))
      vel_y = -kJumpDelta * (kJump - player.state_frame);

    if (player.is(PlayerState::ATTACK_ON_GROUND) ||
        (!input[kInputLeft] && !input[kInputRight])) {
      vel_x *= player.on_ground ? FIX{0.5} : FIX{0.9};
      if (static_cast<int>(vel_x) == 0) vel_x = kZero;
    }

    vel_x = std::clamp(vel_x, FIX{-kMaxVelocityX}, FIX{kMaxVelocityX});
    vel_y = std::clamp(vel_y, -kJumpDelta * kJump, FIX{kMaxVelocityFall});

    // 3. apply
    player.obj.position += player.obj.velocity * FIX{frames};

    // 4. resolve collisions
    for (const auto& platform : platforms_) {
      auto [intersection_x, intersection_y] = isIntersect(player.obj, platform);
      if (intersection_x != kZero && intersection_y != kZero) {
        int x = std::abs(static_cast<int>(intersection_x));
        int y = std::abs(static_cast<int>(intersection_y));
        if (x <= y) player.obj.position.x() += intersection_x;
        if (x >= y) player.obj.position.y() += intersection_y;
      }
    }
    player.on_ground = checkPlatform(player_id);
    if (player.on_ground && vel_y > kZero) vel_y = kZero;
  }

  // 5. resolve damage
  for (int player_id = 0; player_id < kPlayerCount; ++player_id) {
    players_[player_id].on_damage = false;
  }
  for (int i = 0; i < kPlayerCount; ++i) {
    for (int player_id = 0; player_id < kPlayerCount; ++player_id) {
      if (player_id == i) continue;
      auto [x, y] = isIntersect(melee_attack[i], players_[player_id].obj);
      if (x != kZero && y != kZero) {
        players_[player_id].on_damage = true;
      }
    }
  }
}

GameState GameState::getStateProjection() {
  std::scoped_lock lock{mutex_};
  return *this;
};

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
};  // namespace platformer
