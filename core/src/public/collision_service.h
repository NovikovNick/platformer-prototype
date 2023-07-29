#ifndef PLATFORMER_COLLISION_SERVICE_H
#define PLATFORMER_COLLISION_SERVICE_H

#include "game_object.h"

namespace platformer {

class CollistionService {
 public:
  static std::pair<FIX, FIX> isIntersect(const GameObject& lhs,
                                         const GameObject& rhs);
  static void resolveCollision(GameObject& subject, const GameObject& object);
};

};      // namespace platformer
#endif  // PLATFORMER_COLLISION_SERVICE_H