#include <stdio.h>
#include <string.h>
#include <utils.h>

#include "test-util.h"

void
test_hsk_to_lower() {
  printf("  test_hsk_to_lower\n");

  typedef struct {
    uint8_t from;
    uint8_t to;
  } test_case_t;

  test_case_t tests[] = {
    { .from = 'a', .to = 'a' },
    { .from = 'b', .to = 'b' },
    { .from = 'c', .to = 'c' },
    { .from = 'e', .to = 'e' },
    { .from = 'x', .to = 'x' },
    { .from = 'y', .to = 'y' },
    { .from = 'z', .to = 'z' },
    { .from = 'A', .to = 'a' },
    { .from = 'B', .to = 'b' },
    { .from = 'C', .to = 'c' },
    { .from = 'E', .to = 'e' },
    { .from = 'X', .to = 'x' },
    { .from = 'Y', .to = 'y' },
    { .from = 'Z', .to = 'z' },
    { .from = '\x00', .to = '\x00' },
    { .from = '\xff', .to = '\xff' },
    { .from = '@', .to = '@' },
    { .from = '#', .to = '#' },
    { .from = '!', .to = '!' },
    { .from = '.', .to = '.' },
    { .from = '-', .to = '-' },
    { .from = '+', .to = '+' },
    { .from = '_', .to = '_' },
  };

  int i;
  for (i = 0; i < ARRAY_SIZE(tests); i++) {
    test_case_t test = tests[i];

    ASSERT_INT_EQ(hsk_to_lower(test.from), test.to);
  }

  uint8_t j;
  for (j = 0; j < 'A'; j++)
    ASSERT_INT_EQ(hsk_to_lower(j), j);

  for (j = 'A'; j <= 'Z'; j++)
    ASSERT_INT_NEQ(hsk_to_lower(j), j);

  for (j = 0xff; j > 'Z'; j--)
    ASSERT_INT_EQ(hsk_to_lower(j), j);
}

void
test_hsk_name_to_lower() {
  printf("  test_hsk_name_to_lower\n");

  uint8_t upper[][10] =   {"\x08""EXCITING", "\x08""exciting"};
  uint8_t lower[][10] =   {"\x06""boring",   "\x06""boring"};
  uint8_t mixed[][10] =   {"\x06""InSaNe",   "\x06""insane"};
  uint8_t special[][10] = {"\x06""A!b&C ",   "\x06""a!b&c "};

  hsk_name_to_lower(&upper[0][0]);
  hsk_name_to_lower(&lower[0][0]);
  hsk_name_to_lower(&mixed[0][0]);
  hsk_name_to_lower(&special[0][0]);

  ASSERT_INT_EQ(memcmp(&upper[0][0], &upper[1][0], upper[0][0] + 1), 0);
  ASSERT_INT_EQ(memcmp(&lower[0][0], &lower[1][0], lower[0][0] + 1), 0);
  ASSERT_INT_EQ(memcmp(&mixed[0][0], &mixed[1][0], mixed[0][0] + 1), 0);
  ASSERT_INT_EQ(memcmp(&special[0][0], &special[1][0], special[0][0] + 1), 0);
}

void
test_utils() {
  printf("util.h tests...\n");

  test_hsk_to_lower();
  test_hsk_name_to_lower();
}
