#include "ggpo/network/stun_client.h"

#include <stdio.h>
#include <stdlib.h>

#include "ggpo/network/exception.h"
#include "ggpo/network/stun_protocol.h"

namespace net {

StunClient::StunClient(UdpSocket &socket) : socket(socket) {}

Endpoint StunClient::getMappedAddress(const Endpoint &stun_server_endpoint) {
  
  memset(buf, '\0', BUFLEN);

  PACKET stun_request{};
  PACKET *request_ptr = &stun_request;
  packet_init(request_ptr, buf, BUFLEN);
  stun_write_header(request_ptr, STUN_BINDING_METHOD);
  stun_write_footer(request_ptr);

  socket.write(stun_server_endpoint, request_ptr->buf, request_ptr->len);

  memset(buf, '\0', BUFLEN);

  PACKET stun_response;
  PACKET *response_ptr = &stun_response;
  packet_init(response_ptr, buf, BUFLEN);
  for (int attempt = 0; attempt < 10; ++attempt) {
    if (socket.read(response_ptr->buf, BUFLEN) >= 0)
      return stun_parse(response_ptr);
  }
  throw StunServerUnavalableException();
}
}  // namespace net
