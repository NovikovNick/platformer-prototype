#ifndef NET_ENDPOINT_H
#define NET_ENDPOINT_H
#include <string>

namespace net {

struct Endpoint {
  std::string ip;
  int port;
};

}  // namespace net
#endif  // NET_ENDPOINT_H