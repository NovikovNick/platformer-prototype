#include "serializer.h"

#include <schema.pb.h>
#include <util.h>

namespace {

static ser::Vector2 convert(const VECTOR_2& src, const bool fixed) {
  ser::Vector2 dst;
  dst.set_x(fixed ? src.x().raw_value() : src.x().raw_value() >> 16);
  dst.set_y(fixed ? src.y().raw_value() : src.y().raw_value() >> 16);
  return dst;
}
static VECTOR_2 convert(const ser::Vector2& src) {
  return VECTOR_2{FIX::from_raw_value(src.x()), FIX::from_raw_value(src.y())};
}

static ser::GameObject convert(const platformer::GameObject& src,
                               const bool fixed) {
  ser::GameObject dst;

  for (const auto& point : src.mesh)
    dst.mutable_mesh()->Add(convert(point, fixed));
  dst.set_width(src.width_);
  dst.set_height(src.height_);

  *dst.mutable_position() = convert(src.position, fixed);
  *dst.mutable_velocity() = convert(src.velocity, fixed);
  return dst;
}
static platformer::GameObject convert(const ser::GameObject& src) {
  platformer::GameObject dst{src.width(), src.height(),
                             std::vector<VECTOR_2>(src.mesh_size())};
  for (int i = 0; i < src.mesh_size(); ++i) dst.mesh[i] = convert(src.mesh(i));
  dst.position = convert(src.position());
  dst.velocity = convert(src.velocity());
  return dst;
}

static ser::AttackPhase convert(const platformer::AttackPhase& src) {
  switch (src) {
    case platformer::AttackPhase::ACTIVE:
      return ser::AttackPhase::ACTIVE;
    case platformer::AttackPhase::RECOVERY:
      return ser::AttackPhase::RECOVERY;
    case platformer::AttackPhase::STARTUP:
      return ser::AttackPhase::STARTUP;
    case platformer::AttackPhase::NONE:
      return ser::AttackPhase::NONE;
  }
}


static platformer::AttackPhase convert(const ser::AttackPhase& src) {
  switch (src) {
    case ser::AttackPhase::ACTIVE:
      return platformer::AttackPhase::ACTIVE;
    case ser::AttackPhase::RECOVERY:
      return platformer::AttackPhase::RECOVERY;
    case ser::AttackPhase::STARTUP:
      return platformer::AttackPhase::STARTUP;
    case ser::AttackPhase::NONE:
      return platformer::AttackPhase::NONE;
  }
}

static ser::PlayerState convert(const platformer::PlayerState& src) {
  switch (src) {
    case platformer::PlayerState::IDLE:
      return ser::PlayerState::IDLE;
    case platformer::PlayerState::RUN:
      return ser::PlayerState::RUN;
    case platformer::PlayerState::JUMP:
      return ser::PlayerState::JUMP;
    case platformer::PlayerState::FALLING:
      return ser::PlayerState::FALLING;
    case platformer::PlayerState::LANDING:
      return ser::PlayerState::LANDING;
    case platformer::PlayerState::LOW_ATTACK:
      return ser::PlayerState::LOW_ATTACK;
    case platformer::PlayerState::MID_ATTACK:
      return ser::PlayerState::MID_ATTACK;
    case platformer::PlayerState::OVERHEAD_ATTACK:
      return ser::PlayerState::OVERHEAD_ATTACK;
    case platformer::PlayerState::SQUAT:
      return ser::PlayerState::SQUAT;
    case platformer::PlayerState::SQUAT_BLOCK:
      return ser::PlayerState::SQUAT_BLOCK;
    case platformer::PlayerState::BLOCK:
      return ser::PlayerState::BLOCK;
    case platformer::PlayerState::HIT_STUN:
      return ser::PlayerState::HIT_STUN;
    case platformer::PlayerState::BLOCK_STUN:
      return ser::PlayerState::BLOCK_STUN;
    case platformer::PlayerState::SQUAT_BLOCK_STUN:
      return ser::PlayerState::SQUAT_BLOCK_STUN;
    case platformer::PlayerState::DEATH:
      return ser::PlayerState::DEATH;
  }
}
static platformer::PlayerState convert(const ser::PlayerState& src) {
  switch (src) {
    case ser::PlayerState::IDLE:
      return platformer::PlayerState::IDLE;
    case ser::PlayerState::RUN:
      return platformer::PlayerState::RUN;
    case ser::PlayerState::JUMP:
      return platformer::PlayerState::JUMP;
    case ser::PlayerState::FALLING:
      return platformer::PlayerState::FALLING;
    case ser::PlayerState::LANDING:
      return platformer::PlayerState::LANDING;
    case ser::PlayerState::LOW_ATTACK:
      return platformer::PlayerState::LOW_ATTACK;
    case ser::PlayerState::MID_ATTACK:
      return platformer::PlayerState::MID_ATTACK;
    case ser::PlayerState::OVERHEAD_ATTACK:
      return platformer::PlayerState::OVERHEAD_ATTACK;
    case ser::PlayerState::SQUAT:
      return platformer::PlayerState::SQUAT;
    case ser::PlayerState::SQUAT_BLOCK:
      return platformer::PlayerState::SQUAT_BLOCK;
    case ser::PlayerState::BLOCK:
      return platformer::PlayerState::BLOCK;
    case ser::PlayerState::HIT_STUN:
      return platformer::PlayerState::HIT_STUN;
    case ser::PlayerState::BLOCK_STUN:
      return platformer::PlayerState::BLOCK_STUN;
    case ser::PlayerState::SQUAT_BLOCK_STUN:
      return platformer::PlayerState::SQUAT_BLOCK_STUN;
    case ser::PlayerState::DEATH:
      return platformer::PlayerState::DEATH;
  }
}

static ser::Player convert(const platformer::Player& src, const bool fixed) {
  ser::Player dst;
  *dst.mutable_obj() = convert(src.obj, fixed);
  dst.set_state(convert(src.state));
  dst.set_state_frame(src.state_frame);
  dst.set_prev_input(src.prev_input);
  dst.set_left_direction(src.left_direction);
  dst.set_on_ground(src.on_ground);
  dst.set_on_damage(src.on_damage);
  dst.set_current_health(src.current_health);
  dst.set_max_health(src.max_health);
  dst.set_attack_phase(convert(src.attack_phase));
  return dst;
}
static platformer::Player convert(const ser::Player& src) {
  platformer::Player dst;
  dst.obj = convert(src.obj());
  dst.state = convert(src.state());
  dst.state_frame = src.state_frame();
  dst.prev_input = src.prev_input();
  dst.left_direction = src.left_direction();
  dst.on_ground = src.on_ground();
  dst.on_damage = src.on_damage();
  dst.current_health = src.current_health();
  dst.max_health = src.max_health();
  dst.attack_phase = convert(src.attack_phase());
  return dst;
}

static ser::GameState convert(const platformer::GameState& src,
                              const bool fixed) {
  ser::GameState dst;
  dst.set_frame(src.frame);

  for (const auto& player : src.players_)
    dst.mutable_players()->Add(convert(player, fixed));

  for (const auto& platform : src.platforms_)
    dst.mutable_platforms()->Add(convert(platform, fixed));

  for (const auto& attack : src.melee_attack)
    dst.mutable_melee_attacks()->Add(convert(attack, fixed));

  return dst;
}
}  // namespace

namespace platformer {

bool Serializer::serialize(std::shared_ptr<GameState> gs,
                           unsigned char** buffer, int* len) {
  auto lock = gs->lock();
  auto serialized = convert(*gs.get(), true);
  *len = serialized.ByteSize();
  *buffer = new unsigned char[*len];
  return serialized.SerializeToArray(*buffer, serialized.ByteSize());
}

bool Serializer::deserialize(std::shared_ptr<GameState> gs,
                             unsigned char* buffer, int len) {
  ser::GameState serialized;
  auto res = serialized.ParseFromArray(buffer, len);

  auto lock = gs->lock();
  gs->players_.clear();
  gs->platforms_.clear();
  gs->melee_attack.clear();

  for (const auto& player : serialized.players())
    gs->players_.push_back(convert(player));

  for (const auto& platform : serialized.platforms())
    gs->platforms_.push_back(convert(platform));

  for (const auto& it : serialized.melee_attacks())
    gs->melee_attack.push_back(convert(it));

  gs->frame = serialized.frame();
  return res;
}

ser::GameState Serializer::deserialize(unsigned char* buffer, int len) {
  ser::GameState dst;
  dst.ParseFromArray(buffer, len);
  return dst;
}

int Serializer::serialize(const GameState& gs, unsigned char* buffer) {
  auto serialized = convert(gs, false);
  serialized.SerializeToArray(buffer, serialized.ByteSize());
  return serialized.ByteSize();
}
};  // namespace platformer