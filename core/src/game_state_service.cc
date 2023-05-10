#include "game_state_service.h"

#define STATE_DATA(state) (state_data[static_cast<int>(PlayerState::state)])
#define FRAME_DATA(state) (frame_data[static_cast<int>(PlayerState::state)])

namespace {
void updateAttackPhase(platformer::Player& player,
                       platformer::GameObject& attack,
                       const int startup_frame,
                       const int active_frame,
                       const int length) {
  if (player.state_frame < startup_frame) {
    player.attack_phase = platformer::AttackPhase::STARTUP;

    auto progress = FIX{player.state_frame + 1} / FIX{startup_frame};
    attack.width_ = static_cast<int>(progress * length);

  } else if (player.state_frame >= startup_frame &&
             player.state_frame < startup_frame + active_frame) {
    player.attack_phase = platformer::AttackPhase::ACTIVE;

  } else {
    player.attack_phase = platformer::AttackPhase::RECOVERY;
    attack.width_ = std::max<int>(0, 0.9 * attack.width_);
  }
}
}  // namespace

namespace platformer {

GameStateService::GameStateService() : state_data(15), frame_data(15) {
  auto nothing = [](Player& player, GameObject& attack) {};
  auto _default = [](Player& player, GameObject& attack) {
    player.attack_phase = AttackPhase::NONE;
    attack.width_ = 0;
    attack.height_ = 0;
  };
  auto on_jump = [](Player& player, GameObject& attack) {
    player.obj.velocity.y() = -kJumpDelta * (kJump - player.state_frame);
  };
  auto on_overhead = [](Player& player, GameObject& attack) {
    updateAttackPhase(player, attack, 14, 2, 160);

    auto x = player.obj.position.x();
    auto y = player.obj.position.y();
    auto dir_offset = (player.left_direction ? -attack.width_ : 0);

    attack.height_ = player.obj.height_;
    attack.position.x() = dir_offset + x + player.obj.width_ / 2;
    attack.position.y() = y;
  };
  auto on_mid_attack = [](Player& player, GameObject& attack) {
    updateAttackPhase(player, attack, 7, 2, 96);

    auto x = player.obj.position.x();
    auto y = player.obj.position.y();
    auto dir_offset = (player.left_direction ? -attack.width_ : 0);

    attack.height_ = player.obj.height_ / 3;
    attack.position.x() = dir_offset + x + player.obj.width_ / 2;
    attack.position.y() = y;
  };
  auto on_low_attack = [](Player& player, GameObject& attack) {
    updateAttackPhase(player, attack, 14, 2, 128);

    auto x = player.obj.position.x();
    auto y = player.obj.position.y();
    auto dir_offset = (player.left_direction ? -attack.width_ : 0);

    attack.height_ = player.obj.height_;
    attack.position.x() = dir_offset + x + player.obj.width_ / 2;
    attack.position.y() = y;
  };

  // moveable, is_duck, on_state, on_state_out
  STATE_DATA(IDLE) = {true, false, _default, nothing};
  STATE_DATA(RUN) = {true, false, _default, nothing};
  STATE_DATA(JUMP) = {true, false, on_jump, nothing};
  STATE_DATA(FALLING) = {true, false, _default, nothing};
  STATE_DATA(LANDING) = {true, false, _default, nothing};
  STATE_DATA(SQUAT) = {false, true, _default, nothing};
  STATE_DATA(OVERHEAD_ATTACK) = {false, false, on_overhead, nothing};
  STATE_DATA(MID_ATTACK) = {false, false, on_mid_attack, nothing};
  STATE_DATA(LOW_ATTACK) = {false, true, on_low_attack, nothing};
  STATE_DATA(BLOCK) = {false, false, _default, nothing};
  STATE_DATA(SQUAT_BLOCK) = {false, true, _default, nothing};
  STATE_DATA(HIT_STUN) = {false, false, _default, nothing};
  STATE_DATA(BLOCK_STUN) = {false, false, _default, nothing};
  STATE_DATA(SQUAT_BLOCK_STUN) = {false, false, _default, nothing};
  STATE_DATA(DEATH) = {false, false, _default, nothing};

  // damage, hit_damage;
  FRAME_DATA(OVERHEAD_ATTACK) = {10, 2};
  FRAME_DATA(MID_ATTACK) = {2, 1};
  FRAME_DATA(LOW_ATTACK) = {15, 5};
};

const PlayerStateData& GameStateService::getStateData(
    const PlayerState state) const {
  return state_data[(int)state];
};

const bool GameStateService::isAttack(const PlayerState state) const {
  return state == PlayerState::OVERHEAD_ATTACK || state == PlayerState::MID_ATTACK ||
         state == PlayerState::LOW_ATTACK;
};

const FrameData& GameStateService::getFrameData(const PlayerState state) const {
  return frame_data[static_cast<int>(state)];
};

};  // namespace platformer
