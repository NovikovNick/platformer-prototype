#ifndef PLATFORMER_SHOWCASE_UTIL_H
#define PLATFORMER_SHOWCASE_UTIL_H
#include <api.h>
#include <schema.pb.h>

#include <SFML/Graphics.hpp>

#include "../ui/info.h"

platformer::Info initInfo(sf::Color color);

sf::RenderWindow openWindow(const std::string title);

std::string toString(const ser::PlayerState& state);

std::string toString(GameStatus status);

bool wait(const std::chrono::duration<long long, std::milli> duration,
          std::function<bool()> predicate);

#endif  // PLATFORMER_SHOWCASE_UTIL_H