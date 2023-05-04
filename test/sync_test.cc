#include <game_state.h>
#include <serializer.h>
#include <util.h>

#include <bitset>
#include <iostream>

int getRandomInput();

void print(const std::string& title, std::shared_ptr<platformer::GameState> gs);

struct State {
  uint8_t* buf;
  int length, checksum, input1, input2;
  platformer::PlayerState player1, player2;
};

int main() {
  using namespace platformer;
  // Arrange: prepare n game states
  int n = 64;
  std::vector<State> states(n);
  auto gs = std::make_shared<GameState>();
  gs->setPlayerPosition(0, 192, 704);
  gs->setPlayerPosition(1, 96, 704);
  gs->addPlatform(864, 32, 0, 864);
  gs->addPlatform(192, 32, 256, 608);
  gs->addPlatform(224, 32, 672, 736);
  gs->addPlatform(32, 256, 0, 640);
  gs->addPlatform(32, 256, 864, 640);

  for (auto& [buf, length, checksum, input1, input2, p1, p2] : states) {
    if (!Serializer::serialize(gs, &buf, &length)) return 1;
    checksum = fletcher32_checksum((short*)buf, length / 2);
    input1 = getRandomInput();
    input2 = getRandomInput();
    gs->update(input1, input2, 1);
    p1 = gs->players_[0].state;
    p2 = gs->players_[1].state;
  }

  uint8_t* out;
  auto expected_gs = std::make_shared<GameState>();
  for (int i = 0; i < n; ++i) {
    // reproduce rollback: get saved serialized stated and desirialize it to
    // current gs
    auto [buf, l, checksum, input1, input2, p1, p2] = states[i];
    Serializer::deserialize(gs, buf, l);
    for (int j = i + 1; j < n; ++j) {
      // Act: Perform advance frame operation
      gs->update(states[j - 1].input1, states[j - 1].input2, 1);
      if (!Serializer::serialize(gs, &out, &l)) {
        debug("Unable to serialize from {} to {}!\n", i, j);
        return 1;
      }

      // Assert:
      if (states[j - 1].player1 != gs->players_[0].state) {
        debug("Failed player 1 state from {} to {}!\n", i, j);
        debug("Expected {} but was {}\n",
              static_cast<int>(states[j - 1].player1),
              static_cast<int>(gs->players_[0].state));
        return 1;
      }

      if (states[j - 1].player2 != gs->players_[1].state) {
        debug("Failed player 2 state from {} to {}!\n", i, j);
        debug("Expected {} but was {}\n",
              static_cast<int>(states[j - 1].player2),
              static_cast<int>(gs->players_[1].state));
        return 1;
      }

      // Check advansed checksum and expected saved checksum
      if (states[j].checksum != fletcher32_checksum((short*)out, l / 2)) {
        Serializer::deserialize(expected_gs, states[j].buf, states[j].length);
        debug("Failed checksum: from {} to {}!\n", i, j);
        print("EXPECTED", expected_gs);
        print("DESERILIZED", gs);
        return 1;
      }
    }
    debug("{} rollback synchronised with state {} \n", i, static_cast<int>(p1));
  }
  debug("All rollback synchronised!\n");

  return 0;
}

int getRandomInput() {
  std::bitset<6> input(0);
  input[kInputUp] = rand() % 2 == 0;
  input[kInputRight] = rand() % 2 == 0;
  input[kInputDown] = rand() % 2 == 0;
  input[kInputLeft] = rand() % 2 == 0;
  input[kInputLKM] = rand() % 2 == 0;
  input[kInputRKM] = rand() % 2 == 0;
  return input.to_ullong();
}

void print(const std::string& title,
           std::shared_ptr<platformer::GameState> gs) {
  using namespace platformer;
  debug("\n======= {} FRAME#{} =======\n", title, gs->frame);
  for (int i = 0; i < 2; ++i) {
    auto& p = gs->players_[i];
    auto pos_x = static_cast<float>(p.obj.position.x());
    auto pos_y = static_cast<float>(p.obj.position.y());
    auto vel_x = static_cast<float>(p.obj.velocity.x());
    auto vel_y = static_cast<float>(p.obj.velocity.y());

    debug("Player#{}. State {}#{:4d}. ", i, static_cast<int>(p.state),
          p.state_frame);
    debug("Pos [{:8.4f}, {:8.4f}]  ", pos_x, pos_y);
    debug("Vel [{:8.4f}, {:8.4f}]\n", vel_x, vel_y);
  }
}