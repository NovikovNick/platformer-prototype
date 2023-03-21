#ifndef PLATFORMER_SERIALIZER_H
#define PLATFORMER_SERIALIZER_H
#include <game_object.h>
#include <game_state.h>
#include <schema.pb.h>

#include <Eigen/Dense>
#include <fpm/fixed.hpp>

namespace platformer {

class Serializer {
 public:
  static bool serialize(std::shared_ptr<GameState> gs, unsigned char **buffer,
                        int *len);
  static bool deserialize(std::shared_ptr<GameState> gs, unsigned char *buffer,
                          int len);

  static int serialize(const GameState &gs, unsigned char *buffer);

  static ser::GameState deserialize(unsigned char *buffer, int len);
};

};      // namespace platformer
#endif  // PLATFORMER_SERIALIZER_H