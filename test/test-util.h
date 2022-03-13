#ifndef _TEST_UTIL_H
#define _TEST_UTIL_H

#include <stdio.h>
#include <stdint.h>

#define ARRAY_SIZE(x) ((sizeof(x))/(sizeof(x[0])))

inline void
print_array(uint8_t *arr, size_t size){
  for (int i = 0; i < size; i++) {
    printf("%02x", arr[i]);
  }
  printf("\n");
}

// asserts
#define ASSERT_BASE(expr, a, operator, b, type, conv)         \
 do {                                                         \
  if (!(expr)) {                                              \
    fprintf(stderr,                                           \
            "Assertion failed in %s on line %d: `%s %s %s` "  \
            "(%"#conv" %s %"#conv")\n",                       \
            __FILE__,                                         \
            __LINE__,                                         \
            #a,                                               \
            #operator,                                        \
            #b,                                               \
            (type)a,                                          \
            #operator,                                        \
            (type)b);                                         \
    fflush(stdout);                                           \
    abort();                                                  \
  }                                                           \
 } while (0)

#define ASSERT_INT_BASE(a, operator, b, type, conv)           \
 ASSERT_BASE((a) operator (b), a, operator, b, type, conv)

#define ASSERT_INT_EQ(a, b) ASSERT_INT_BASE(a, ==, b, int, d)
#define ASSERT_INT_NEQ(a, b) ASSERT_INT_BASE(a, !=, b, int, d)

#define ASSERT_BOOL_EQ(a, b) ASSERT_INT_BASE(a, ==, b, bool, d)
#define ASSERT_BOOL_NEQ(a, b) ASSERT_INT_BASE(a, !=, b, bool, d)

#define ASSERT(expr) ASSERT_BOOL_EQ(expr, true)

#endif // _TEST_UTIL_H
