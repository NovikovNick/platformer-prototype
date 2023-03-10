#include "net_game_loop.h"

#include <ggponet.h>
#include <serializer.h>
#include <util.h>
// #include <windows.h>

#include <chrono>
#include <thread>

#include "nongamestate.h"

namespace {
std::shared_ptr<platformer::GameState> game_state;
NonGameState ngs;
GGPOSession *ggpo;

int fletcher32_checksum(short *data, size_t len) {
  int sum1 = 0xffff, sum2 = 0xffff;

  while (len) {
    size_t tlen = len > 360 ? 360 : len;
    len -= tlen;
    do {
      sum1 += *data++;
      sum2 += sum1;
    } while (--tlen);
    sum1 = (sum1 & 0xffff) + (sum1 >> 16);
    sum2 = (sum2 & 0xffff) + (sum2 >> 16);
  }

  /* Second reduction step to reduce sums to 16 bits */
  sum1 = (sum1 & 0xffff) + (sum1 >> 16);
  sum2 = (sum2 & 0xffff) + (sum2 >> 16);
  return sum2 << 16 | sum1;
}

/*
 * vw_begin_game_callback --
 *
 * The begin game callback.  We don't need to do anything special here,
 * so just return true.
 */
bool __cdecl vw_begin_game_callback(const char *) { return true; }

/*
 * vw_on_event_callback --
 *
 * Notification from GGPO that something has happened.  Update the status
 * text at the bottom of the screen to notify the user.
 */
bool __cdecl vw_on_event_callback(GGPOEvent *info) {
  int progress;
  int player_id;
  switch (info->code) {
    case GGPO_EVENTCODE_CONNECTED_TO_PEER:
      player_id = info->u.connected.player;
      ngs.SetConnectState(player_id, Synchronizing);
      platformer::debug("NGS: {} synchronizing\n", player_id);
      break;
    case GGPO_EVENTCODE_SYNCHRONIZING_WITH_PEER:
      progress =
          100 * info->u.synchronizing.count / info->u.synchronizing.total;
      player_id = info->u.connected.player;
      ngs.UpdateConnectProgress(player_id, progress);
      platformer::debug("NGS: {} sync: {}\n", player_id, progress);
      break;
    case GGPO_EVENTCODE_SYNCHRONIZED_WITH_PEER:
      player_id = info->u.connected.player;
      ngs.UpdateConnectProgress(player_id, 100);
      platformer::debug("NGS: {} synchronized\n", player_id);
      break;
    case GGPO_EVENTCODE_RUNNING:
      ngs.SetConnectState(Running);
      platformer::debug("NGS: {} running\n");
      break;
    case GGPO_EVENTCODE_CONNECTION_INTERRUPTED:
      player_id = info->u.connection_interrupted.player;
      /*ngs.SetDisconnectTimeout(
          player_id, timeGetTime(),
          info->u.connection_interrupted.disconnect_timeout);*/
      platformer::debug("NGS: {} interrupted. Not implemented\n", player_id);
      break;
    case GGPO_EVENTCODE_CONNECTION_RESUMED:
      player_id = info->u.connection_interrupted.player;
      ngs.SetConnectState(player_id, Running);
      platformer::debug("NGS: {} resumed\n", player_id);
      break;
    case GGPO_EVENTCODE_DISCONNECTED_FROM_PEER:
      player_id = info->u.disconnected.player;
      ngs.SetConnectState(player_id, Disconnected);
      platformer::debug("NGS: {} disconnected\n", player_id);
      break;
    case GGPO_EVENTCODE_TIMESYNC:
      /*Sleep(1000 * info->u.timesync.frames_ahead / 60);
        auto player_id = info->u.disconnected.player;*/
      platformer::debug("NGS: {} tymesync. Not implemented\n",
                        info->u.timesync.frames_ahead);
      break;
  }
  return true;
}

/*
 * vw_advance_frame_callback --
 *
 * Notification from GGPO we should step foward exactly 1 frame
 * during a rollback.
 */
bool __cdecl vw_advance_frame_callback(int) {
  const int player_count = 2;
  int inputs[player_count] = {0};
  int disconnect_flags;

  // Make sure we fetch new inputs from GGPO and use those to update
  // the game state instead of reading from the keyboard.
  /*ggpo_synchronize_input(ggpo, (void *)inputs, sizeof(int) * player_count,
                         &disconnect_flags);*/
  // VectorWar_AdvanceFrame(inputs, disconnect_flags);
  return true;
}

/*
 * vw_load_game_state_callback --
 *
 * Makes our current state match the state passed in by GGPO.
 */
bool __cdecl vw_load_game_state_callback(unsigned char *buffer, int len) {
  // memcpy(&gs, buffer, len);
  platformer::Serializer::deserialize(game_state, buffer, len);
  return true;
}

/*
 * vw_save_game_state_callback --
 *
 * Save the current state to a buffer and return it to GGPO via the
 * buffer and len parameters.
 */
bool __cdecl vw_save_game_state_callback(unsigned char **buffer, int *len,
                                         int *checksum, int) {
  bool res = platformer::Serializer::serialize(game_state, buffer, len);
  if (!res) return false;
  *checksum = fletcher32_checksum((short *)*buffer, *len / 2);
  return true;
}

/*
 * vw_log_game_state --
 *
 * Log the gamestate.  Used by the synctest debugging tool.
 */
bool __cdecl vw_log_game_state(char *filename, unsigned char *buffer, int) {
  FILE *fp = nullptr;
  /*fopen_s(&fp, filename, "w");
  if (fp) {
    GameState *gamestate = (GameState *)buffer;
    fprintf(fp, "GameState object.\n");
    fprintf(fp, "  bounds: %d,%d x %d,%d.\n", gamestate->_bounds.left,
            gamestate->_bounds.top, gamestate->_bounds.right,
            gamestate->_bounds.bottom);
    fprintf(fp, "  num_ships: %d.\n", gamestate->_num_ships);
    for (int i = 0; i < gamestate->_num_ships; i++) {
      Ship *ship = gamestate->_ships + i;
      fprintf(fp, "  ship %d position:  %.4f, %.4f\n", i, ship->position.x,
              ship->position.y);
      fprintf(fp, "  ship %d velocity:  %.4f, %.4f\n", i, ship->velocity.dx,
              ship->velocity.dy);
      fprintf(fp, "  ship %d radius:    %d.\n", i, ship->radius);
      fprintf(fp, "  ship %d heading:   %d.\n", i, ship->heading);
      fprintf(fp, "  ship %d health:    %d.\n", i, ship->health);
      fprintf(fp, "  ship %d speed:     %d.\n", i, ship->speed);
      fprintf(fp, "  ship %d cooldown:  %d.\n", i, ship->cooldown);
      fprintf(fp, "  ship %d score:     %d.\n", i, ship->score);
      for (int j = 0; j < MAX_BULLETS; j++) {
        Bullet *bullet = ship->bullets + j;
        fprintf(fp, "  ship %d bullet %d: %.2f %.2f -> %.2f %.2f.\n", i, j,
                bullet->position.x, bullet->position.y, bullet->velocity.dx,
                bullet->velocity.dy);
      }
    }
    fclose(fp);
  }*/
  return true;
}

/*
 * vw_free_buffer --
 *
 * Free a save state buffer previously returned in vw_save_game_state_callback.
 */
void __cdecl vw_free_buffer(void *buffer) { free(buffer); }
}  // namespace

namespace platformer {

using namespace std::chrono;
using clock = high_resolution_clock;
using frame = duration<uint64_t, std::ratio<1, 60>>;

NetGameLoop::NetGameLoop(std::shared_ptr<GameState> gs,
                         std::shared_ptr<std::atomic<int>> tick,
                         std::shared_ptr<std::atomic<int>> tick_rate,
                         std::shared_ptr<std::atomic<float>> tick_ratio,
                         std::shared_ptr<std::atomic<int>> p0_input,
                         std::shared_ptr<std::atomic<int>> p1_input)
    : frame_(0),
      tick_(tick),
      tick_rate_(tick_rate),
      tick_ratio_(tick_ratio),
      p0_input_(p0_input),
      p1_input_(p1_input),
      running_(false) {
  game_state = gs;

  GGPOSessionCallbacks cb = {0};
  cb.begin_game = vw_begin_game_callback;
  cb.advance_frame = vw_advance_frame_callback;
  cb.load_game_state = vw_load_game_state_callback;
  cb.save_game_state = vw_save_game_state_callback;
  cb.free_buffer = vw_free_buffer;
  cb.on_event = vw_on_event_callback;
  cb.log_game_state = vw_log_game_state;

  const int num_players = 2;
  unsigned short port = 7000;
  auto input_size = sizeof(int);
  const char *title = "platformer";
  GGPOErrorCode res;
  res = ggpo_start_session(&ggpo, &cb, title, num_players, input_size, port);
  ggpo_set_disconnect_timeout(ggpo, 3000);
  ggpo_set_disconnect_notify_start(ggpo, 1000);
  debug("GGPO session started\n");
  /*
  WSADATA wd = {0};
  WSAStartup(MAKEWORD(2, 2), &wd);
  */
  GGPOPlayer players[num_players];
  players[0].size = sizeof(players[0]);
  players[0].player_num = 1;
  players[0].type = GGPO_PLAYERTYPE_LOCAL;

  GGPOPlayerHandle handle;
  res = ggpo_add_player(ggpo, players + 0, &handle);
  ngs.players[0].handle = handle;
  ngs.players[0].type = players[0].type;
  ngs.players[0].connect_progress = 100;
  ngs.local_player_handle = handle;
  ngs.SetConnectState(handle, Connecting);
  ggpo_set_frame_delay(ggpo, handle, 2);

  // ngs.players[i].connect_progress = 0;
};

void NetGameLoop::operator()() {
  running_ = true;
  auto t0 = clock::now();
  auto t1 = clock::now();
  auto t2 = clock::now();
  float dx = 0;
  float micro = 0;
  int frame_per_tick, tick_rate;

  GGPOErrorCode result = GGPO_OK;

  while (running_) {
    ggpo_idle(ggpo, 0);

    t1 = clock::now();
    auto next_frame = duration_cast<frame>(t1 - t0).count();
    micro = duration_cast<microseconds>(t1 - t2).count();
    tick_rate = std::clamp(tick_rate_->load(), 1, 60);
    frame_per_tick = 60 / tick_rate;

    // the ratio between the elapsed microseconds and the number of microseconds
    // in 1 tick
    tick_ratio_->store(micro / (1e6 / tick_rate));

    if (frame_ != next_frame) {
      frame_ = next_frame;

      if (next_frame % frame_per_tick == 0) {
        auto input = p0_input_->load();
        // result = ggpo_add_local_input(ggpo, ngs.local_player_handle, &input,
        // sizeof(input));
        tick_->fetch_add(1);
        game_state->update(input, p1_input_->load(), 1);
        t2 = t1;
      }
    }
  }
};

};  // namespace platformer