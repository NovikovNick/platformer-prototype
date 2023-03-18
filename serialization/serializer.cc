#include "serializer.h"

#include <schema.pb.h>
#include <util.h>

namespace {

static ser::Vector2 convert(const VECTOR_2& src) {
  ser::Vector2 dst;
  dst.set_x(src.x().raw_value());
  dst.set_y(src.y().raw_value());
  return dst;
}
static VECTOR_2 convert(const ser::Vector2& src) {
  return VECTOR_2{FIX::from_raw_value(src.x()), FIX::from_raw_value(src.y())};
}

static ser::GameObject convert(const platformer::GameObject& src) {
  ser::GameObject dst;

  for (const auto& point : src.mesh) dst.mutable_mesh()->Add(convert(point));
  dst.set_width(src.width_);
  dst.set_height(src.height_);

  *dst.mutable_position() = convert(src.position);
  *dst.mutable_velocity() = convert(src.velocity);
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
    case platformer::PlayerState::ATTACK_ON_GROUND:
      return ser::PlayerState::ATTACK_ON_GROUND;
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
    case ser::PlayerState::ATTACK_ON_GROUND:
      return platformer::PlayerState::ATTACK_ON_GROUND;
    case ser::PlayerState::DEATH:
      return platformer::PlayerState::DEATH;
  }
}

static ser::Player convert(const platformer::Player& src) {
  ser::Player dst;
  *dst.mutable_obj() = convert(src.obj);
  dst.set_state(convert(src.state));
  dst.set_state_frame(src.state_frame);
  dst.set_prev_input(src.prev_input);
  dst.set_left_direction(src.left_direction);
  dst.set_on_ground(src.on_ground);
  dst.set_on_damage(src.on_damage);
  return dst;
}
static platformer::Player convert(const ser::Player& src) {
  platformer::Player dst;
  dst.obj = convert(src.obj());
  dst.state = convert(src.state());
  dst.state_frame  = src.state_frame();
  dst.prev_input = src.prev_input();
  dst.left_direction = src.left_direction();
  dst.on_ground = src.on_ground();
  dst.on_damage = src.on_damage();
  return dst;
}

static ser::GameState convert(const platformer::GameState& src) {
  ser::GameState dst;

  for (const auto& player : src.players_)
    dst.mutable_players()->Add(convert(player));

  for (const auto& platform : src.platforms_)
    dst.mutable_platforms()->Add(convert(platform));

  return dst;
}
}  // namespace

namespace platformer {

bool Serializer::serialize(std::shared_ptr<GameState> gs,
                           unsigned char** buffer, int* len) {
  auto lock = gs->lock();
  auto serialized = convert(*gs.get());
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

  for (const auto& player : serialized.players())
    gs->players_.push_back(convert(player));

  for (const auto& platform : serialized.platforms())
    gs->platforms_.push_back(convert(platform));

  return res;
}

};  // namespace platformer