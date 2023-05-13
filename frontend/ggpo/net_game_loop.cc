#include "net_game_loop.h"

#include <ggponet.h>
#include <serializer.h>
#include <util.h>
#include <windows.h>

#include <chrono>
#include <thread>

#include "nongamestate.h"

namespace {
std::shared_ptr<platformer::GameState> game_state;
NonGameState ngs;
GGPOSession *ggpo;
int local_player, remote_player;

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
      progress = 100 * info->u.synchronizing.count / info->u.synchronizing.total;
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
      platformer::debug("NGS: running\n");
      break;
    case GGPO_EVENTCODE_CONNECTION_INTERRUPTED:
      player_id = info->u.connection_interrupted.player;
      ngs.SetDisconnectTimeout(player_id,
                               timeGetTime(),
                               info->u.connection_interrupted.disconnect_timeout);
      platformer::debug("NGS: {} interrupted\n", player_id);
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
      Sleep(1000 * info->u.timesync.frames_ahead / 60);
      auto player_id = info->u.disconnected.player;
      platformer::debug("NGS: {} tymesync.\n", info->u.timesync.frames_ahead);
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
  ggpo_synchronize_input(
      ggpo, (void *)inputs, sizeof(int) * player_count, &disconnect_flags);
  game_state->update(inputs[0], inputs[1]);

  // Notify ggpo that we've moved forward exactly 1 frame.
  ggpo_advance_frame(ggpo);
  platformer::debug("vw_advance_frame_callback\n");
  return true;
}

/*
 * vw_load_game_state_callback --
 *
 * Makes our current state match the state passed in by GGPO.
 */
bool __cdecl vw_load_game_state_callback(unsigned char *buffer, int len) {
  platformer::Serializer::deserialize(game_state, buffer, len);
  platformer::debug("deserialized\n");
  return true;
}

/*
 * vw_save_game_state_callback --
 *
 * Save the current state to a buffer and return it to GGPO via the
 * buffer and len parameters.
 */
bool __cdecl vw_save_game_state_callback(unsigned char **buffer,
                                         int *len,
                                         int *checksum,
                                         int) {
  bool res = platformer::Serializer::serialize(game_state, buffer, len);
  if (!res) return false;
  *checksum = platformer::fletcher32_checksum((short *)*buffer, *len / 2);
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
void __cdecl vw_free_buffer(void *buffer) {
  // platformer::debug("free buffer\n");
  free(buffer);
}

void print(const uint64_t frame, GGPOErrorCode result) {
  std::string res_str;
  switch (result) {
    case GGPO_OK: res_str = "GGPO_OK"; break;
    case GGPO_ERRORCODE_GENERAL_FAILURE:
      res_str = "GGPO_ERRORCODE_GENERAL_FAILURE";
      break;
    case GGPO_ERRORCODE_INVALID_SESSION:
      res_str = "GGPO_ERRORCODE_INVALID_SESSION";
      break;
    case GGPO_ERRORCODE_INVALID_PLAYER_HANDLE:
      res_str = "GGPO_ERRORCODE_INVALID_PLAYER_HANDLE";
      break;
    case GGPO_ERRORCODE_PLAYER_OUT_OF_RANGE:
      res_str = "GGPO_ERRORCODE_PLAYER_OUT_OF_RANGE";
      break;
    case GGPO_ERRORCODE_PREDICTION_THRESHOLD:
      res_str = "GGPO_ERRORCODE_PREDICTION_THRESHOLD";
      break;
    case GGPO_ERRORCODE_UNSUPPORTED: res_str = "GGPO_ERRORCODE_UNSUPPORTED"; break;
    case GGPO_ERRORCODE_NOT_SYNCHRONIZED:
      res_str = "GGPO_ERRORCODE_NOT_SYNCHRONIZED";
      break;
    case GGPO_ERRORCODE_IN_ROLLBACK: res_str = "GGPO_ERRORCODE_IN_ROLLBACK"; break;
    case GGPO_ERRORCODE_INPUT_DROPPED:
      res_str = "GGPO_ERRORCODE_INPUT_DROPPED";
      break;
    case GGPO_ERRORCODE_PLAYER_DISCONNECTED:
      res_str = "GGPO_ERRORCODE_PLAYER_DISCONNECTED";
      break;
    case GGPO_ERRORCODE_INVALID_REQUEST:
      res_str = "GGPO_ERRORCODE_PLAYER_DISCONNECTED";
      break;
    default: res_str = std::to_string(static_cast<int>(result));
  }
  platformer::debug("{:5d}: {}\n", frame, res_str);
}
}  // namespace

namespace platformer {

using namespace std::chrono;
using clock = high_resolution_clock;
using frame = duration<uint64_t, std::ratio<1, 60>>;

NetGameLoop::NetGameLoop(InputArgs args,
                         std::shared_ptr<GameState> gs,
                         std::shared_ptr<std::atomic<int>> p0_input,
                         std::shared_ptr<std::atomic<int>> p1_input,
                         std::shared_ptr<std::atomic<bool>> running,
                         std::shared_ptr<std::atomic<int>> status)
    : frame_(0 - 1),
      p0_input_(p0_input),
      p1_input_(p1_input),
      running_(running),
      status_(status) {
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
  const int frame_delay = 3;
  auto input_size = sizeof(int);
  const char *title = "platformer";

  ngs.num_players = num_players;

  if (args.local) {
    local_player = 0;
    remote_player = 1;
  } else {
    local_player = 1;
    remote_player = 0;
  }
  GGPOErrorCode res;

  WSADATA wd = {0};
  WSAStartup(MAKEWORD(2, 2), &wd);
  res = ggpo_start_session(
      &ggpo, &cb, title, num_players, input_size, args.local_port);
  ggpo_set_disconnect_timeout(ggpo, 3000);
  ggpo_set_disconnect_notify_start(ggpo, 1000);

  GGPOPlayer players[num_players];
  players[local_player].size = sizeof(players[local_player]);
  players[local_player].player_num = local_player + 1;
  players[local_player].type = GGPO_PLAYERTYPE_LOCAL;

  players[remote_player].size = sizeof(players[remote_player]);
  players[remote_player].player_num = remote_player + 1;
  players[remote_player].type = GGPO_PLAYERTYPE_REMOTE;
  for (int i = 0; i < 32; ++i)
    players[remote_player].u.remote.ip_address[i] = args.ip[i];
  players[remote_player].u.remote.port = args.remote_port;

  for (int i = 0; i < num_players; i++) {
    GGPOPlayerHandle handle;
    auto addr = players + i;
    res = ggpo_add_player(ggpo, addr, &handle);
    ngs.players[i].handle = handle;
    ngs.players[i].type = players[i].type;
    if (players[i].type == GGPO_PLAYERTYPE_LOCAL) {
      ngs.players[i].connect_progress = 100;
      ngs.local_player_handle = handle;
      ngs.SetConnectState(handle, Connecting);
      ggpo_set_frame_delay(ggpo, handle, frame_delay);
    } else {
      ngs.players[i].connect_progress = 0;
    }
  }
  status_->store(1);

  debug("Connecting to peers\n");
}
NetGameLoop::~NetGameLoop() { WSACleanup(); };

void NetGameLoop::operator()() {
  if (timeBeginPeriod(1) == TIMERR_NOERROR) {
    debug("Minimum resolution for periodic timers has been updated to 1ms\n");
  } else {
    debug(
        "Unable to set minimum resolution for periodic timers in windows! App "
        "will work in busy loop.\n");
  }

  auto started_time = clock::now();
  auto current_time = clock::now();
  const float frame_time = getMicrosecondsInOneTick();
  int update_time = 0;
  int sleep_time = 0;

  microseconds running_time;
  frame frames;
  uint64_t startup_offset;

  GGPOErrorCode result = GGPO_OK;
  int disconnect_flags;
  int inputs[2] = {0};
  int input;

  while (running_->load()) {
    running_time = duration_cast<microseconds>(current_time - started_time);
    frames = duration_cast<frame>(running_time);
    startup_offset =
        running_time.count() - duration_cast<microseconds>(frames).count();

    if (frame_ + 1 != frames.count())
      debug("Frame mismatch {} => {}!\n", frame_, frames.count());

    if (frame_ != frames.count()) {
      frame_ = frames.count();

      {  // update
        result = ggpo_idle(ggpo, sleep_time);

        if (ngs.local_player_handle != GGPO_INVALID_HANDLE) {
          input = local_player == 0 ? p0_input_->load() : p1_input_->load();
          result = ggpo_add_local_input(
              ggpo, ngs.local_player_handle, &input, sizeof(input));
        }
        // print(frame_, result);

        // synchronize these inputs with ggpo.  If we have enough input to
        // proceed ggpo will modify the input list with the correct inputs to
        // use and return 1.
        if (GGPO_SUCCEEDED(result)) {
          result = ggpo_synchronize_input(
              ggpo, (void *)inputs, sizeof(int) * 2, &disconnect_flags);
          if (GGPO_SUCCEEDED(result)) {
            // inputs[0] and inputs[1] contain the inputs for p1 and p2. Advance
            // the game by 1 frame using those inputs.
            game_state->update(inputs[0], inputs[1]);
            status_->store(0);

            // update the checksums to display in the top of the window.  this
            // helps to detect desyncs.
            ngs.now.framenumber = frame_;
            /*ngs.now.checksum =
                fletcher32_checksum((short *)&gs, sizeof(gs) / 2);
            if ((gs._framenumber % 90) == 0) {
              ngs.periodic = ngs.now;
            }*/
            ggpo_advance_frame(ggpo);
          }
        }
      }

      update_time = duration_cast<microseconds>(clock::now() - current_time).count();
      sleep_time = std::ceil((frame_time - update_time - startup_offset) / 1000);
      if (sleep_time > 0) Sleep(sleep_time);
      current_time = clock::now();
    } else {
      Sleep(1);
      current_time = clock::now();
    }
  }
  ggpo_close_session(ggpo);
  timeEndPeriod(1);
  status_->store(2);
};

long long NetGameLoop::getMicrosecondsInOneTick() {
  return duration_cast<microseconds>(frame(1)).count();
};

};  // namespace platformer