#include "game_state.h"

#include <util.h>

#include <bitset>
#include <sml.hpp>

namespace {

inline FIX length(const FIX& x, const FIX& y) {
  return FIX{std::sqrt(std::pow(static_cast<int>(x), 2) +
                       std::pow(static_cast<int>(y), 2))};
}

inline VECTOR_2 normal(const VECTOR_2& val) {
  if (val.x() == kZero && val.y() == kZero) return {kZero, kZero};
  return val / length(val.x(), val.y());
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

struct InputLKM {};
struct InputLeft {};
struct InputRight {};
struct InputUp {};
struct InputDown {};
struct InputNone {};

class idle;
class run;
class jump;
class falling;
class landing;
class attack_on_ground;
class death;

const auto onGround = [](const Player& ctx) { return ctx.on_ground; };
const auto inAir = [](const Player& ctx) { return !ctx.on_ground; };
struct FrameLessOrEq {
  int frame;
  auto operator()(const Player& ctx) const { return ctx.state_frame <= frame; }
};
struct FrameGreat {
  int frame;
  auto operator()(const Player& ctx) const { return ctx.state_frame > frame; }
};

auto idle_s = boost::sml::state<idle>;
auto run_s = boost::sml::state<run>;
auto jump_s = boost::sml::state<jump>;
auto falling_s = boost::sml::state<falling>;
auto landing_s = boost::sml::state<landing>;
auto attack_s = boost::sml::state<attack_on_ground>;
auto death_s = boost::sml::state<death>;

auto input_up = boost::sml::event<InputUp>;
auto none = boost::sml::event<InputNone>;
auto input_left = boost::sml::event<InputLeft>;
auto input_right = boost::sml::event<InputRight>;
auto input_attack = boost::sml::event<InputLKM>;

struct transition_table {
  auto operator()() const {
    using namespace boost::sml;
    /**
     * Initial state: *initial_state
     * Transition DSL: src_state + event [ guard ] / action = dst_state
     */
    return make_transition_table(
        *idle_s + none[onGround] = idle_s,
        idle_s + input_left[onGround] = run_s,
        idle_s + input_right[onGround] = run_s,
        idle_s + input_up[onGround] = jump_s,
        idle_s + input_attack[onGround] = attack_s,
        idle_s + none[inAir] = falling_s,

        landing_s + none[onGround && FrameLessOrEq{2}] = idle_s,
        landing_s + none[onGround && FrameGreat{2}] = idle_s,
        landing_s + none[inAir] = falling_s,

        attack_s + none[onGround && FrameLessOrEq{kAttack - 1}] = attack_s,
        attack_s + none[onGround && FrameGreat{kAttack - 1}] = idle_s,
        attack_s + none[inAir] = falling_s,

        run_s + none[onGround] = idle_s,
        run_s + input_left[onGround] = run_s,
        run_s + input_right[onGround] = run_s,
        run_s + input_up[onGround] = jump_s,
        run_s + input_attack[onGround] = attack_s,
        run_s + none[inAir] = falling_s,

        jump_s + none[inAir && FrameLessOrEq{kJump - 1}] = jump_s,
        jump_s + none[inAir && FrameGreat{kJump - 1}] = falling_s,
        jump_s + none[onGround] = landing_s,

        falling_s + none[onGround] = landing_s);
  }
};

PlayerState getState(const auto& sm) {
  if (sm.is(idle_s)) return PlayerState::IDLE;
  if (sm.is(run_s)) return PlayerState::RUN;
  if (sm.is(jump_s)) return PlayerState::JUMP;
  if (sm.is(falling_s)) return PlayerState::FALLING;
  if (sm.is(landing_s)) return PlayerState::LANDING;
  if (sm.is(attack_s)) return PlayerState::ATTACK_ON_GROUND;
  return PlayerState::DEATH;
}

std::vector<boost::sml::sm<transition_table>> fsms_;

GameState::GameState()
    : players_(std::vector<Player>{}),
      melee_attack(std::vector<GameObject>{}),
      platforms_(std::vector<GameObject>{}) {
  players_.emplace_back().obj.position = {FIX(192), FIX(768)};
  players_.emplace_back().obj.position = {FIX(96), FIX(768)};

  std::vector<VECTOR_2> mesh{
      {kZero, kOne}, {kOne, kOne}, {kOne, kZero}, {kZero, kZero}};
  platforms_.emplace_back(864, 32, mesh).position = {FIX(0), FIX(864)};
  platforms_.emplace_back(192, 32, mesh).position = {FIX(256), FIX(608)};
  platforms_.emplace_back(224, 32, mesh).position = {FIX(672), FIX(736)};
  platforms_.emplace_back(32, 256, mesh).position = {FIX(0), FIX(640)};
  platforms_.emplace_back(32, 256, mesh).position = {FIX(864), FIX(640)};

  melee_attack.emplace_back(0, 0, mesh);
  melee_attack.emplace_back(0, 0, mesh);

  using namespace boost::sml;
  fsms_.emplace_back(players_[0]);
  fsms_.emplace_back(players_[1]);
}

GameState::GameState(GameState& src) {
  players_ = src.players_;
  platforms_ = src.platforms_;
  melee_attack = src.melee_attack;
}

void GameState::update(const int p0_input, const int p1_input,
                       const int frames) {
  int player_count = 2;
  std::scoped_lock lock{mutex_};

  for (int player_id = 0; player_id < 2; ++player_id) {
    auto& player = players_[player_id];
    auto& fsm = fsms_[player_id];

    auto& vel_x = player.obj.velocity.x();
    auto& vel_y = player.obj.velocity.y();

    std::bitset<5> input(player_id == 0 ? p0_input : p1_input);
    std::bitset<5> prev_input(player.prev_input);

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
    player.prev_input = input.to_ulong();

    player.updateState(getState(fsm));

    // 2. update input
    if (player.is(PlayerState::FALLING)) vel_y += FIX{kAccelerationGravity};
    if (player.is(PlayerState::ATTACK_ON_GROUND)) {
      melee_attack[player_id].position.x() = player.obj.position.x();
      melee_attack[player_id].position.y() =
          player.obj.position.y() - player.obj.height_ / 2;
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
