#ifndef PLATFORMER_FRONEND_API_H
#define PLATFORMER_FRONEND_API_H
#define EXPORT extern "C" __declspec(dllexport)

struct Input {
  bool leftPressed, rightPressed, upPressed, downPressed, leftMouseClicked;
};

EXPORT void StartGame();

EXPORT void StopGame();

EXPORT void Update(const Input input);

EXPORT int GetState(unsigned char* buf);

#endif  // PLATFORMER_FRONEND_API_H