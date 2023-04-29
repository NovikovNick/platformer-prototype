#ifndef PLATFORMER_LOCOMOTION_FSM_H
#define PLATFORMER_LOCOMOTION_FSM_H

#include <player.h>

#include <sml.hpp>

namespace platformer {

struct InputLKM {};
struct InputRKM {};
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

struct FrameLessOrEq {
  int frame;
  auto operator()(const Player& ctx) const { return ctx.state_frame <= frame; }
};
struct FrameGreat {
  int frame;
  auto operator()(const Player& ctx) const { return ctx.state_frame > frame; }
};

struct player_locomotion_table {
  auto operator()() const {
    using namespace boost::sml;
    const auto onGround = [](const Player& ctx) { return ctx.on_ground; };
    const auto inAir = [](const Player& ctx) { return !ctx.on_ground; };

    auto idle_s = state<idle>;
    auto run_s = state<run>;
    auto jump_s = state<jump>;
    auto falling_s = state<falling>;
    auto landing_s = state<landing>;
    auto attack_s = state<attack_on_ground>;
    auto death_s = state<death>;

    auto input_up = event<InputUp>;
    auto none = event<InputNone>;
    auto input_left = event<InputLeft>;
    auto input_right = event<InputRight>;
    auto input_attack = event<InputLKM>;

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

        run_s + none[onGround] = idle_s, run_s + input_left[onGround] = run_s,
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

using PlayerLocomotionFSM =
    std::vector<boost::sml::sm<player_locomotion_table>>;

PlayerState getState(const auto& sm) {
  using namespace boost::sml;
  if (sm.is(state<idle>)) return PlayerState::IDLE;
  if (sm.is(state<run>)) return PlayerState::RUN;
  if (sm.is(state<jump>)) return PlayerState::JUMP;
  if (sm.is(state<falling>)) return PlayerState::FALLING;
  if (sm.is(state<landing>)) return PlayerState::LANDING;
  if (sm.is(state<attack_on_ground>)) return PlayerState::ATTACK_ON_GROUND;
  return PlayerState::DEATH;
}
};      // namespace platformer
#endif  // PLATFORMER_LOCOMOTION_FSM_H