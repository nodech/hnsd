#include <stdio.h>
#include <base32.h>

#include "test-util.h"

void
test_base32_encode_decode() {
  printf("  test_base32_encode_decode\n");

  const char *str = "5l6tm80";
  const uint8_t expected[4] = {45, 77, 219, 32};

  uint8_t ip[4];
  hsk_base32_decode_hex(str, ip, false);
  for (int i = 0; i < 4; i++) {
    ASSERT_INT_EQ(ip[i], expected[i]);
  }

  char encoded[8];
  hsk_base32_encode_hex(ip, 4, encoded, false);
  ASSERT_INT_EQ(strcmp(encoded, str), 0);
}


void
test_base32() {
  printf("base32.h tests...\n");

  test_base32_encode_decode();
}
