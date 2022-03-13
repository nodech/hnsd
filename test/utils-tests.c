#include <stdio.h>
#include <string.h>
#include <utils.h>

#include "test-util.h"

void
test_hsk_to_lower() {
  printf("  test_hsk_to_lower\n");

  uint8_t upper[][10] =   {"\x08""EXCITING", "\x08""exciting"};
  uint8_t lower[][10] =   {"\x06""boring",   "\x06""boring"};
  uint8_t mixed[][10] =   {"\x06""InSaNe",   "\x06""insane"};
  uint8_t special[][10] = {"\x06""A!b&C ",   "\x06""a!b&c "};

  hsk_to_lower(&upper[0][0]);
  hsk_to_lower(&lower[0][0]);
  hsk_to_lower(&mixed[0][0]);
  hsk_to_lower(&special[0][0]);

  ASSERT_INT_EQ(memcmp(&upper[0][0], &upper[1][0], upper[0][0] + 1), 0);
  ASSERT_INT_EQ(memcmp(&lower[0][0], &lower[1][0], lower[0][0] + 1), 0);
  ASSERT_INT_EQ(memcmp(&mixed[0][0], &mixed[1][0], mixed[0][0] + 1), 0);
  ASSERT_INT_EQ(memcmp(&special[0][0], &special[1][0], special[0][0] + 1), 0);
}

void
test_utils() {
  printf("util.h tests...\n");

  test_hsk_to_lower();
}
