#include <stdio.h>
#include <string.h>
#include <hash.h>

#include "test-util.h"

void
test_hsk_hash_tld() {
  printf("  test_hsk_hash_tld\n");

  const uint8_t name1[10] = "\x09""handshake";
  const uint8_t expected[] = {
    0x3a, 0xa2, 0x52, 0x85, 0x76, 0xf9, 0x6b, 0xd4,
    0x0f, 0xcf, 0xf0, 0xbd, 0x6b, 0x60, 0xc4, 0x42,
    0x21, 0xd7, 0x3c, 0x43, 0xb4, 0xe4, 0x2d, 0x4b,
    0x90, 0x8e, 0xd2, 0x0a, 0x93, 0xb8, 0xd1, 0xb6
  };

  uint8_t actual[32];
  hsk_hash_tld((uint8_t *)&name1, (uint8_t *)&actual);
  ASSERT_INT_EQ(memcmp(&actual, &expected, 32), 0);
}

void
test_hash() {
  printf("hash.h tests...\n");

  test_hsk_hash_tld();
}
