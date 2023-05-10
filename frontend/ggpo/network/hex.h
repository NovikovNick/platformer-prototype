#ifndef NET_HEX_H
#define NET_HEX_H

int hex_encode(char *dest, char *src, int len) {
  const char *table = "0123456789abcdef";
  int j;
  for (j = 0; j < len; j++) {
    dest[j * 2] = table[((src[j] >> 4) & 0xF)];
    dest[j * 2 + 1] = table[(src[j]) & 0x0F];
  }
  dest[len * 2] = 0;
  return len * 2;
}

void hex_decode(char *dest, char *src, int len) {
  unsigned char v = 0;
  char *d = dest;
  char *p = src;
  int res;

  while ((res = sscanf(p, "%02x", &v)) > 0) {
    *d++ = v;
    p += res * 2;
  }
}

char *hex_string(char *buf, int len) {
  static char msg[256];
  hex_encode(msg, buf, len);
  return msg;
}

#endif  // NET_HEX_H