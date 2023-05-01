#include "util.h"

namespace platformer {

int fletcher32_checksum(short* data, size_t len) {
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
};  // namespace platformer