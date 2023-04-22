#ifndef NET_STUN_PROTOCOL_H
#define NET_STUN_PROTOCOL_H

#include <stdio.h>
#include <stdlib.h>

#include "endpoint.h"
#include "hex.h"

namespace net {

enum stun_vars {
  STUN_MAPPED_ADDRESS = 0x0001,
  STUN_RESPONSE_ADDRESS = 0x0002,
  STUN_CHANGE_REQUEST = 0x0003,
  STUN_SOURCE_ADDRESS = 0x0004,
  STUN_CHANGED_ADDRESS = 0x0005,
  STUN_USERNAME = 0x0006,
  STUN_PASSWORD = 0x0007,
  STUN_MESSAGE_INTEGRITY = 0x0008,
  STUN_ERROR_CODE = 0x0009,
  STUN_UNKNOWN_ATTRIBUTES = 0x000A,
  STUN_REFLECTED_FROM = 0x000B,
  STUN_CHANNEL_NUMBER = 0x000C,
  STUN_LIFETIME = 0x000D,
  STUN_BANDWIDTH = 0x0010,
  STUN_PEER_ADDRESS = 0x0012,
  STUN_DATA = 0x0013,
  STUN_REALM = 0x0014,
  STUN_NONCE = 0x0015,
  STUN_RELAYED_ADDRESS = 0x0016,
  STUN_REQUESTED_ADDRESS_TYPE = 0x0017,
  STUN_REQUESTED_PROPS = 0x0018,
  STUN_REQUESTED_TRANSPORT = 0x0019,
  STUN_XOR_MAPPED_ADDRESS = 0x0020,
  STUN_TIMER_VAL = 0x0021,
  STUN_RESERVATION_TOKEN = 0x0022,
  STUN_XOR_REFLECTED_FROM = 0x0023,
  STUN_PRIORITY = 0x0024,
  STUN_USE_CANDIDATE = 0x0025,
  STUN_ICMP = 0x0030,
  STUN_END_MANDATORY_ATTR,
  STUN_START_EXTENDED_ATTR = 0x8021,
  STUN_SOFTWARE = 0x8022,
  STUN_ALTERNATE_SERVER = 0x8023,
  STUN_REFRESH_INTERVAL = 0x8024,
  STUN_FINGERPRINT = 0x8028,
  STUN_ICE_CONTROLLED = 0x8029,
  STUN_ICE_CONTROLLING = 0x802A,
};

enum {
  STUN_HEADER_SIZE = 20,
  STUN_MAGIC_COOKIE = 0x2112A442,
  STUN_BINDING_METHOD = 1,
  STUN_SHARED_SECRET_METHOD = 2,
  STUN_ALLOCATE_METHOD = 3,
  STUN_REFRESH_METHOD = 4,
  STUN_SEND_METHOD = 6,
  STUN_DATA_METHOD = 7,
  STUN_CHANNEL_BIND_METHOD = 9,
  STUN_REQUEST_CLASS = 0,
  STUN_INDICATION_CLASS = 1,
  STUN_SUCCESS_CLASS = 2,
  STUN_ERROR_CLASS = 3,
};

#define ADDRESS struct sockaddr_in

char *net_to_string(ADDRESS *addr) {
  static char msg[64];
  unsigned char *ip = (unsigned char *)&addr->sin_addr.s_addr;
  sprintf(msg, "%d.%d.%d.%d:%d", ip[0], ip[1], ip[2], ip[3],
          ntohs(addr->sin_port));
  return msg;
}

typedef struct {
  char *buf;
  int ofs;
  int size;
  int len;
} PACKET;

int packet_init(PACKET *m, char *buf, int buf_size) {
  m->buf = buf;
  m->size = buf_size;
  m->len = 0;
  m->ofs = 0;
  return 0;
}

int packet_write(PACKET *m, int v) {
  if (m && m->ofs < m->size - 1) {
    m->buf[m->ofs++] = v & 0xff;

    if (m->len < m->ofs) m->len++;

    return 1;
  }
  return 0;
}

unsigned long long packet_read(PACKET *m) {
  return (m && (m->ofs < m->size - 1)) ? m->buf[m->ofs++] & 0xff : 0;
}

#define w8(m, v) packet_write(m, v & 0xff)
#define w16(m, v) w8(m, v >> 8) + w8(m, v)
#define w32(m, v) w16(m, v >> 16) + w16(m, v)
#define w64(m, v) w32(m, v >> 32) + w32(m, v)
#define wBuf(m, buf, len)                         \
  {                                               \
    int k = 0, i = 0;                             \
    for (i = 0; i < len; i++) k += w8(m, buf[i]); \
  }
#define r8(m) packet_read(m)
#define r16(m) (((r8(m) << 8) | r8(m)) & 0xffff)
#define r32(m) ((r16(m) << 16) | r16(m))
#define r64(m) ((r32(m) << 32) | r32(m))
#define rBuf(m, buf, len)                     \
  {                                           \
    int i = 0;                                \
    for (i = 0; i < len; i++) buf[i] = r8(m); \
  }

void random_data(char *buf, int a, int b, int len) {
  int i;
  for (i = 0; i < len; i++) buf[i] = a + rand() % (b - a);
}

void stun_write_header(PACKET *m, int type) {
  char tsx_id[12];
  random_data(tsx_id, 0, 0xff, 12);
  w16(m, type);
  w16(m, 0);
  w32(m, STUN_MAGIC_COOKIE);
  wBuf(m, tsx_id, 12);
}

void stun_write_footer(PACKET *m) {
  m->ofs = 2;
  w16(m, m->len - STUN_HEADER_SIZE);
}

int stun_xor_address(ADDRESS *addr) {
  int i;
  int x = htonl(STUN_MAGIC_COOKIE);
  char *p = (char *)&x;
  int msb = ((char *)&x)[0] << 8 | ((char *)&x)[1];
  addr->sin_port ^= htons(msb);
  char *ip = (char *)&addr->sin_addr.s_addr;
  for (i = 0; i < 4; i++) ip[i] ^= p[i];
  return 0;
}

int stun_parse_address(PACKET *m, ADDRESS *addr) {
  addr->sin_family = r16(m) == 1 ? 2 : 1;
  addr->sin_port = htons(r16(m));
  char *p = (char *)&addr->sin_addr.s_addr;
  rBuf(m, p, 4);
  return 0;
}

Endpoint stun_parse(PACKET *m) {
  m->ofs = 0;
  int type = r16(m);
  int length = r16(m);
  int magic = r32(m);
  char tsx_id[12];

  if (magic != STUN_MAGIC_COOKIE) throw StunResponseParseException();

  rBuf(m, tsx_id, 12);

  int msg = type & ~0x110;
  int code = type & 0x110;

  // printf(" Message: %d (%d)\n", msg, code);
  // printf("  hdr: length=%d, magic=0x%x, tsx_id=%s", length, magic,
  // hex_string(tsx_id, 12)); printf("\n"); printf("  Attributes:\n");

  int offset = m->ofs;

  while ((offset - STUN_HEADER_SIZE) < length) {
    int attr = r16(m);
    int len = r16(m);

    // printf(" 0x%04x length=%d, ", attr, len);

    switch (attr) {
      case STUN_MAPPED_ADDRESS:
      case STUN_RESPONSE_ADDRESS:
      case STUN_SOURCE_ADDRESS:
      case STUN_CHANGED_ADDRESS:
      case STUN_XOR_MAPPED_ADDRESS: {
        ADDRESS addr;

        stun_parse_address(m, &addr);

        if (attr == STUN_XOR_MAPPED_ADDRESS) stun_xor_address(&addr);

        // std::string endpoint = net_to_string(&addr);
        unsigned char *ip = (unsigned char *)&addr.sin_addr.s_addr;
        return {std::format("{:d}.{:d}.{:d}.{:d}", ip[0], ip[1], ip[2], ip[3]),
                ntohs(addr.sin_port)};
      }

      case STUN_SOFTWARE:
        printf(m->buf + m->ofs);
        break;

      default:
        printf(hex_string(m->buf + m->ofs, len));
        break;
    }

    printf("\n");
    // len = round_int(len, 4);
    offset += len + 4;
    m->ofs = offset;
  }

  throw StunResponseParseException();
}

}  // namespace net

#endif  // NET_STUN_PROTOCOL_H
