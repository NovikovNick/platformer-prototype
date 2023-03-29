#ifndef PLATFORMER_NET_ARGS_H
#define PLATFORMER_NET_ARGS_H

struct InputArgs {
  bool local;
  unsigned short local_port;

  char ip[32];
  unsigned short remote_port;
};

#endif