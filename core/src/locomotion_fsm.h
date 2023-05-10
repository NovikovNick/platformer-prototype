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
struct InputBackwardLKM {};
struct InputDownLKM {};
struct InputDownRKM {};

class initial;
class idle;
class run;
class jump;
class falling;
class landing;
class low_attack;
class mid_attack;
class overhead_attack;
class squat;
class block;
class squat_block;
class hit_stun;
class block_stun;
class squat_block_stun;
class death;

struct FrameLessOrEq {
  int frame;
  auto operator()(const Player& ctx) const { return ctx.state_frame <= frame; }
};
struct FrameGreat {
  int frame;
  auto operator()(const Player& ctx) const { return ctx.state_frame > frame; }
};
struct IsState {
  PlayerState state;
  auto operator()(const Player& ctx) const { return ctx.state == state; }
};
struct OnDamage {
  bool state;
  auto operator()(const Player& ctx) const { return ctx.on_damage == state; }
};

struct player_locomotion_table {
  auto operator()() const {
    using namespace boost::sml;
    const auto onGround = [](const Player& ctx) { return ctx.on_ground; };
    const auto inAir = [](const Player& ctx) { return !ctx.on_ground; };

    auto initial_s = state<initial>;
    auto idle_s = state<idle>;
    auto run_s = state<run>;
    auto jump_s = state<jump>;
    auto falling_s = state<falling>;
    auto landing_s = state<landing>;
    auto low_attack_s = state<low_attack>;
    auto mid_attack_s = state<mid_attack>;
    auto overhead_attack_s = state<overhead_attack>;
    auto squat_s = state<squat>;
    auto block_s = state<block>;
    auto squat_block_s = state<squat_block>;
    auto hit_stun_s = state<hit_stun>;
    auto block_stun_s = state<block_stun>;
    auto squat_block_stun_s = state<squat_block_stun>;
    auto death_s = state<death>;

    auto none = event<InputNone>;
    auto input_up = event<InputUp>;
    auto input_down = event<InputDown>;
    auto input_left = event<InputLeft>;
    auto input_right = event<InputRight>;
    auto input_lkm = event<InputLKM>;
    auto input_rkm = event<InputRKM>;
    auto input_down_lkm = event<InputDownLKM>;
    auto input_down_rkm = event<InputDownRKM>;
    auto input_backward_lkm = event<InputBackwardLKM>;

    /**
     * Initial state: *initial_state
     * Transition DSL: src_state + event [ guard ] / action = dst_state
     */
    return make_transition_table(
        *initial_s + none[IsState{PlayerState::IDLE}] = idle_s,
        initial_s + none[IsState{PlayerState::RUN}] = run_s,
        initial_s + none[IsState{PlayerState::JUMP}] = jump_s,
        initial_s + none[IsState{PlayerState::FALLING}] = falling_s,
        initial_s + none[IsState{PlayerState::LANDING}] = landing_s,
        initial_s + none[IsState{PlayerState::BLOCK}] = block_s,
        initial_s + none[IsState{PlayerState::SQUAT}] = squat_s,
        initial_s + none[IsState{PlayerState::SQUAT_BLOCK}] = squat_block_s,
        initial_s + none[IsState{PlayerState::LOW_ATTACK}] = low_attack_s,
        initial_s + none[IsState{PlayerState::MID_ATTACK}] = mid_attack_s,
        initial_s + none[IsState{PlayerState::OVERHEAD_ATTACK}] = overhead_attack_s,
        initial_s + none[IsState{PlayerState::HIT_STUN}] = hit_stun_s,
        initial_s + none[IsState{PlayerState::BLOCK_STUN}] = block_stun_s,
        initial_s + none[IsState{PlayerState::SQUAT_BLOCK_STUN}] = squat_block_stun_s,
        initial_s + none[IsState{PlayerState::DEATH}] = death_s,

        idle_s + none[onGround] = idle_s,
        idle_s + input_left[onGround] = run_s,
        idle_s + input_right[onGround] = run_s,
        idle_s + input_up[onGround] = jump_s,
        idle_s + input_rkm[onGround] = block_s,
        idle_s + input_down[onGround] = squat_s,
        idle_s + input_down_rkm[onGround] = squat_block_s,
        idle_s + input_lkm[onGround] = mid_attack_s,
        idle_s + input_down_lkm[onGround] = low_attack_s,
        idle_s + input_backward_lkm[onGround] = overhead_attack_s,
        idle_s + none[inAir] = falling_s,

        hit_stun_s + none[FrameLessOrEq{kHitStun - 1}] = hit_stun_s,
        hit_stun_s + none[FrameGreat{kHitStun - 1}] = idle_s,
        
        squat_block_stun_s + none[FrameLessOrEq{kBlockStun - 1}] = squat_block_stun_s,
        squat_block_stun_s + none[FrameGreat{kBlockStun - 1}] = idle_s,

        block_stun_s + none[FrameLessOrEq{kBlockStun - 1}] = block_stun_s,
        block_stun_s + none[FrameGreat{kBlockStun - 1}] = idle_s,

        squat_block_s + none[onGround] = idle_s,

        block_s + none[onGround] = idle_s,

        squat_s + none[onGround] = idle_s,
        squat_s + input_down_rkm[onGround] = squat_block_s,
        squat_s + input_lkm[onGround] = low_attack_s,
        squat_s + input_down_lkm[onGround] = low_attack_s,

        landing_s + none[onGround && FrameLessOrEq{2}] = idle_s,
        landing_s + none[onGround && FrameGreat{2}] = idle_s,
        landing_s + none[inAir] = falling_s,

        mid_attack_s + none[onGround && FrameLessOrEq{kMidAttack - 1}] = mid_attack_s,
        mid_attack_s + none[onGround && FrameGreat{kMidAttack - 1}] = idle_s,
        mid_attack_s + none[inAir] = falling_s,

        low_attack_s + none[onGround && FrameLessOrEq{kLowAttack - 1}] = low_attack_s,
        low_attack_s + none[onGround && FrameGreat{kLowAttack - 1}] = idle_s,
        low_attack_s + none[inAir] = falling_s,
        
        overhead_attack_s + none[onGround && FrameLessOrEq{kOverHeadAttack - 1}] = overhead_attack_s,
        overhead_attack_s + none[onGround && FrameGreat{kOverHeadAttack - 1}] = idle_s,
        overhead_attack_s + none[inAir] = falling_s,

        run_s + none[onGround] = idle_s, 
        run_s + input_left[onGround] = run_s,
        run_s + input_right[onGround] = run_s,
        run_s + input_up[onGround] = jump_s,
        run_s + input_rkm[onGround] = block_s,
        run_s + input_down_rkm[onGround] = squat_block_s,
        run_s + input_down[onGround] = squat_s,
        run_s + input_lkm[onGround] = mid_attack_s,
        run_s + input_down_lkm[onGround] = low_attack_s,
        run_s + input_backward_lkm[onGround] = overhead_attack_s,
        run_s + none[inAir] = falling_s,

        jump_s + none[inAir && FrameLessOrEq{kJump - 1}] = jump_s,
        jump_s + none[inAir && FrameGreat{kJump - 1}] = falling_s,
        jump_s + none[onGround] = landing_s,

        falling_s + none[onGround] = landing_s,
        falling_s + input_up[inAir && FrameLessOrEq{2}] = jump_s);
  }
};

using PlayerSM = boost::sml::sm<player_locomotion_table>;

PlayerState getState(const auto& sm) {
  using namespace boost::sml;
  if (sm.is(state<idle>)) return PlayerState::IDLE;
  if (sm.is(state<run>)) return PlayerState::RUN;
  if (sm.is(state<jump>)) return PlayerState::JUMP;
  if (sm.is(state<falling>)) return PlayerState::FALLING;
  if (sm.is(state<landing>)) return PlayerState::LANDING;
  if (sm.is(state<low_attack>)) return PlayerState::LOW_ATTACK;
  if (sm.is(state<mid_attack>)) return PlayerState::MID_ATTACK;
  if (sm.is(state<overhead_attack>)) return PlayerState::OVERHEAD_ATTACK;
  if (sm.is(state<squat>)) return PlayerState::SQUAT;
  if (sm.is(state<block>)) return PlayerState::BLOCK;
  if (sm.is(state<squat_block>)) return PlayerState::SQUAT_BLOCK;
  if (sm.is(state<hit_stun>)) return PlayerState::HIT_STUN;
  if (sm.is(state<block_stun>)) return PlayerState::BLOCK_STUN;
  if (sm.is(state<squat_block_stun>)) return PlayerState::SQUAT_BLOCK_STUN;
  return PlayerState::DEATH;
}
};      // namespace platformer
#endif  // PLATFORMER_LOCOMOTION_FSM_H