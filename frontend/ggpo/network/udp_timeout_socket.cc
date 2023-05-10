#include "exception.h"
#include "udp_socket.h"

namespace net {

UdpSocket::UdpSocket(const int port) {
  WSADATA wsa;
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    throw ConnectionException(WSAGetLastError());

  if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    throw ConnectionException(WSAGetLastError());

  int to = 500;
  int optval = 1;
  setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&to, sizeof(to));
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof optval);

  sockaddr_in local_addr = {0};
  local_addr.sin_family = AF_INET;
  local_addr.sin_addr.s_addr = INADDR_ANY;
  local_addr.sin_port = htons(port);

  if (bind(s, (sockaddr*)&local_addr, sizeof(local_addr)) == SOCKET_ERROR)
    throw ConnectionException(WSAGetLastError());
}

UdpSocket::~UdpSocket() {
  if (s != INVALID_SOCKET) {
    closesocket(s);
    WSACleanup();
  }
};

int UdpSocket::read(char* buf, int length) {
  int recv_len, slen = sizeof(sockaddr_in);
  sockaddr_in recv_addr;
  return recvfrom(s, buf, length, 0, (sockaddr*)&recv_addr, &slen);
};

void UdpSocket::write(const Endpoint& endpoint, const char* buf, int length) {
  sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(endpoint.ip.c_str());
  addr.sin_port = htons(endpoint.port);

  int recv_len, slen = sizeof(sockaddr_in);
  if (sendto(s, buf, length, 0, (sockaddr*)&addr, slen) == SOCKET_ERROR) {
    throw UnableToSendException(WSAGetLastError());
  }
};

}  // namespace net