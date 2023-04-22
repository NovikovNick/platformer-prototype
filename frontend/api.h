#ifndef PLATFORMER_FRONEND_API_H
#define PLATFORMER_FRONEND_API_H
#define EXPORT extern "C" __declspec(dllexport)

enum GameStatus {
  INVALID   = -1,
  RUN       = 0,
  SYNC      = 1,
  STOPED    = 2,
};

enum PlatformerErrorCode {
  OK = 0,
  UNEXPECTED_ERROR = 1,
  UNABLE_TO_PARSE_STUN_RESPONSE = 2,
};

struct Input {
  bool leftPressed, rightPressed, upPressed, downPressed, leftMouseClicked;
};

struct Endpoint {
  const char* remote_host;
  const int remote_port;
};

EXPORT void Init(const bool is_1st_player); // TODO: add platforms

EXPORT Endpoint GetPublicEndpoint(const int local_port);

EXPORT void RegisterPeer(const Endpoint peer_endpoint);

EXPORT void StartGame();

EXPORT void StopGame();

EXPORT void Update(const Input input);

EXPORT int GetState(unsigned char* buf);

EXPORT GameStatus GetStatus();

EXPORT PlatformerErrorCode GetErrorCode();

#endif  // PLATFORMER_FRONEND_API_H