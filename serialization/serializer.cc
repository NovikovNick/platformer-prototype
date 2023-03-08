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
  return VECTOR_2{FIXED::from_raw_value(src.x()),
                  FIXED::from_raw_value(src.y())};
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
    case platformer::PlayerState::JUMP_DOWN:
      return ser::PlayerState::JUMP_DOWN;
    case platformer::PlayerState::JUMP_UP:
      return ser::PlayerState::JUMP_UP;
    case platformer::PlayerState::RUN:
      return ser::PlayerState::RUN;
  }
}
static platformer::PlayerState convert(const ser::PlayerState& src) {
  switch (src) {
    case ser::PlayerState::IDLE:
      return platformer::PlayerState::IDLE;
    case ser::PlayerState::JUMP_DOWN:
      return platformer::PlayerState::JUMP_DOWN;
    case ser::PlayerState::JUMP_UP:
      return platformer::PlayerState::JUMP_UP;
    case ser::PlayerState::RUN:
      return platformer::PlayerState::RUN;
  }
}

static ser::Player convert(const platformer::Player& src) {
  ser::Player dst;
  *dst.mutable_obj() = convert(src.obj);
  dst.set_state(convert(src.state_));
  dst.set_frame(src.frame_);
  dst.set_on_platform(src.on_platform_);
  return dst;
}
static platformer::Player convert(const ser::Player& src) {
  platformer::Player dst;
  dst.obj = convert(src.obj());
  dst.state_ = convert(src.state());
  dst.frame_ = src.frame();
  dst.on_platform_ = src.on_platform();
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