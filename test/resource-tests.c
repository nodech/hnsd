#include <stdio.h>
#include <resource.h>

#include "test-util.h"

void
test_pointer_to_ip() {
  printf("  test_pointer_to_ip\n");
  const uint8_t str4[] = "\x08""_5l6tm80""\x06""_synth""\x00";
  const uint8_t expected4[4] = {45, 77, 219, 32};
  uint8_t ip4[4];
  uint16_t family4;

  bool ret4 = pointer_to_ip(&str4[0], ip4, &family4);
  ASSERT(ret4);
  for (int i = 0; i < 4; i++) {
    ASSERT_INT_EQ(ip4[i], expected4[i]);
  }

  ASSERT_INT_EQ(family4, HSK_DNS_A);

  const uint8_t str6[] = "\x1b""_400hjs000l2gol000fvvsc9cpg""\x06""_synth""\x00";
  const uint8_t expected6[16] = {
    0x20, 0x01, 0x19, 0xf0,
    0x00, 0x05, 0x45, 0x0c,
    0x54, 0x00, 0x03, 0xff,
    0xfe, 0x31, 0x2c, 0xcc
  };

  uint8_t ip6[16];
  uint16_t family6;

  bool ret6 = pointer_to_ip(&str6[0], ip6, &family6);
  assert(ret6);
  for (int i = 0; i < 16; i++) {
    assert(ip6[i] == expected6[i]);
  }
  assert(family6 == HSK_DNS_AAAA);
}

void
test_resource() {
  printf("resource.h tests...\n");

  test_pointer_to_ip();
}
