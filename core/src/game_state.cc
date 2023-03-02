#include "game_state.h"

#include <bitset>

// #include "player.cc"

namespace {

inline FIXED length(const FIXED& x, const FIXED& y) {
  return FIXED{std::sqrt(std::pow(static_cast<int>(x), 2) +
                         std::pow(static_cast<int>(y), 2))};
}

inline VECTOR_2 normal(const VECTOR_2& val) {
  if (val.x() == kZero && val.y() == kZero) return {kZero, kZero};
  return val / length(val.x(), val.y());
}

std::pair<FIXED, FIXED> isIntersect(const math::GameObject& lhs,
                                    const math::GameObject& rhs) {
  auto [lhs_min_x, lhs_max_x] = lhs.getProjectionMinMax(0);
  auto [rhs_min_x, rhs_max_x] = rhs.getProjectionMinMax(0);
  auto [lhs_min_y, lhs_max_y] = lhs.getProjectionMinMax(1);
  auto [rhs_min_y, rhs_max_y] = rhs.getProjectionMinMax(1);

  // rhs_max_y >= lhs_min_y && rhs_min_y <= lhs_max_y;
  if (rhs_max_x - lhs_min_x < kZero) return {kZero, kZero};
  if (lhs_max_x - rhs_min_x < kZero) return {kZero, kZero};
  if (rhs_max_y - lhs_min_y < kZero) return {kZero, kZero};
  if (lhs_max_y - rhs_min_y < kZero) return {kZero, kZero};

  FIXED diff_x = std::min(rhs_max_x - lhs_min_x, lhs_max_x - rhs_min_x);
  FIXED diff_y = std::min(rhs_max_y - lhs_min_y, lhs_max_y - rhs_min_y);

  if (rhs_max_x > lhs_max_x) diff_x *= -1;
  if (rhs_max_y > lhs_max_y) diff_y *= -1;
  return {diff_x, diff_y};
}
}  // namespace

namespace math {

GameState::GameState()
    : platforms_(std::vector<GameObject>{}), players_(std::vector<Player>{}) {
  players_.emplace_back().obj.position = {FIXED(192), FIXED(768)};
  players_.emplace_back().obj.position = {FIXED(96), FIXED(768)};

  std::vector<VECTOR_2> mesh{
      {kZero, kOne}, {kOne, kOne}, {kOne, kZero}, {kZero, kZero}};
  platforms_.emplace_back(864, 32, mesh).position = {FIXED(0), FIXED(864)};
  platforms_.emplace_back(192, 32, mesh).position = {FIXED(256), FIXED(608)};
  platforms_.emplace_back(224, 32, mesh).position = {FIXED(672), FIXED(736)};
  platforms_.emplace_back(32, 256, mesh).position = {FIXED(0), FIXED(640)};
  platforms_.emplace_back(32, 256, mesh).position = {FIXED(864), FIXED(640)};
}

void GameState::update(const int p0_input, const int p1_input,
                       const int frames) {
  std::scoped_lock lock{mutex_};
  for (int player_id = 0; player_id < 2; ++player_id) {
    auto& player = players_[player_id];
    auto& vel_x = player.obj.velocity.x();
    auto& vel_y = player.obj.velocity.y();
    auto& pos_x = player.obj.position.x();
    auto& pos_y = player.obj.position.y();

    std::bitset<4> set(player_id == 0 ? p0_input : p1_input);
    if (set[kInputLeft]) vel_x += FIXED{-kAccelerationX};
    if (set[kInputRight]) vel_x += FIXED{kAccelerationX};

    // 3. fall
    if (player.state_ == PlayerState::JUMP_DOWN && !player.on_platform_) {
      vel_y += FIXED{kAccelerationGravity};
    }

    // 2. jump
    if (!player.on_platform_) {
      if (player.state_ == PlayerState::JUMP_UP && player.frame_ < kJump) {
        ++player.frame_;
        vel_y = -kJumpDelta * (kJump - player.frame_);
      } else {
        player.updateFrame(PlayerState::JUMP_DOWN);
      }
    }

    // todo: avoid multiple click
    // 1. start jump
    if (set[kInputUp] && player.on_platform_) {
      vel_y = -kJumpDelta * kJump;
      player.state_ = PlayerState::JUMP_UP;
      player.frame_ = 0;
    }

    if (!set[kInputLeft] && !set[kInputRight]) {
      vel_x *= player.on_platform_ ? FIXED{0.5} : FIXED{0.9};
      if (static_cast<int>(vel_x) == 0) vel_x = kZero;
    }

    vel_x = std::clamp(vel_x, FIXED{-kMaxVelocityX}, FIXED{kMaxVelocityX});
    vel_y = std::clamp(vel_y, -kJumpDelta * kJump, FIXED{kMaxVelocityFall});

    player.obj.position += player.obj.velocity * FIXED{frames};

    for (const auto& platform : platforms_) {
      auto [intersection_x, intersection_y] = isIntersect(player.obj, platform);
      if (intersection_x != kZero && intersection_y != kZero) {
        int x = std::abs(static_cast<int>(intersection_x));
        int y = std::abs(static_cast<int>(intersection_y));

        if (x < y) {
          player.obj.position.x() += intersection_x;
        } else if (x > y) {
          player.obj.position.y() += intersection_y;
        } else {
          player.obj.position += VECTOR_2{intersection_x, intersection_y};
        }
      }
    }

    player.on_platform_ = checkPlatform(player_id);
    if (player.on_platform_) {
      player.updateFrame(set[kInputLeft] || set[kInputRight]
                             ? PlayerState::RUN
                             : PlayerState::IDLE);
      if (vel_y > kZero) vel_y = kZero;
    }

    /*switch (state_) {
      case PlayerState::IDLE:
        debug("{:4s}#{:3d} ", "idle", frame_);
        break;
      case PlayerState::RUN:
        debug("{:4s}#{:3d} ", "run", frame_);
        break;
      case PlayerState::JUMP_UP:
        debug("{:4s}#{:3d} ", "up", frame_);
        break;
      case PlayerState::JUMP_DOWN:
        debug("{:4s}#{:3d} ", "down", frame_);
        break;
    }
    debug("pos[{:8.3f},{:8.3f}] vel[{:8.3f},{:8.3f}]\n",
          static_cast<float>(pos_x), static_cast<float>(pos_y),
          static_cast<float>(vel_x), static_cast<float>(vel_y));*/
  }
}

GameObject GameState::getPlayer(const int player_id) {
  std::scoped_lock lock{mutex_};
  return players_[player_id].obj;
}

std::vector<GameObject>& GameState::getPlatforms() { return platforms_; }

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
};  // namespace math
