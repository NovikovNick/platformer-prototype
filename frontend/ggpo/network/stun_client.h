#ifndef NET_CLIENT_H
#define NET_CLIENT_H
#define BUFLEN 1024
#include "endpoint.h"
#include "udp_socket.h"

namespace net {

class StunClient {
  UdpSocket &socket;
  char buf[BUFLEN];

 public:
  StunClient(UdpSocket &socket);
  Endpoint getMappedAddress(const Endpoint &stun_server_endpoint);
};
}  // namespace net
#endif  // NET_CLIENT_H
