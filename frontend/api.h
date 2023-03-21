#ifndef PLATFORMER_FRONEND_API_H
#define PLATFORMER_FRONEND_API_H
#define EXPORT extern "C" __declspec(dllexport)

#include <cstdint>

struct Input {
  bool leftPressed, rightPressed, upPressed, downPressed, leftMouseClicked;
};

EXPORT void StartGame();

EXPORT void StopGame();

EXPORT void Update(const Input input);

EXPORT int GetState(uint8_t* buf);

#endif  // PLATFORMER_SERIALIZER_H