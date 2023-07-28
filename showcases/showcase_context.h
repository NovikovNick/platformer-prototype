#ifndef PLATFORMER_SHOWCASE_CONTEXT_H
#define PLATFORMER_SHOWCASE_CONTEXT_H
#include <api.h>

#include <format>
#include <vector>

namespace platformer {

struct ShowcaseContext {
  int active_player_id;  // 1st of 2nd player
  Point position_1st_player, position_2nd_player;
  std::vector<Platform> platforms;

  uint8_t game_state_buf[512];
  char local_public_ip[32]{0};
  char remote_public_ip[32]{0};
  char remote_public_ip_val[32]{0};

  int tick_rate;
  bool log_dx, log_player_state;

  bool ctrl_pressed = false;
  bool right_mouse_pressed = false;
  int prev_mouse_x = 0, prev_mouse_y = 0;
  int screen_offset_x = 0, screen_offset_y = 1000;
  double scale = 1.0;

  ShowcaseContext()
      : tick_rate(60),
        log_dx(false),
        log_player_state(false),
        active_player_id(0),
        position_1st_player({192, 704}),
        position_2nd_player({96, 704}),
        platforms({{0, 0, 1864, 32, {0, 864}},
                   {1, 0, 192, 32, {256, 608}},
                   {2, 0, 224, 32, {672, 736}},
                   {3, 0, 32, 256, {0, 640}},
                   {4, 0, 32, 256, {1864, 640}}}) {}

  void setLocalPublicEndpoint(const Endpoint endpoint) {
    std::string endpoint_str(
        std::format("{}:{}", endpoint.remote_host, endpoint.remote_port));
    strcpy(local_public_ip, endpoint_str.c_str());
  };

  Endpoint getRemoteEndpoint() {
    std::string remote_ip(remote_public_ip);
    if (!remote_ip.empty()) {
      auto colon = remote_ip.find(':');
      std::string remote_host = remote_ip.substr(0, colon);
      int remote_port = std::stoi(remote_ip.substr(colon + 1));
      strcpy(remote_public_ip_val, remote_host.c_str());
      return {remote_public_ip_val, remote_port};
    }
    return {};
  }

  Location getLocation() {
    return {active_player_id == 0,
            position_1st_player,
            position_2nd_player,
            platforms.data(),
            platforms.size()};
  }
};

};      // namespace platformer
#endif  // PLATFORMER_SHOWCASE_CONTEXT_H