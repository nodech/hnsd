#include <stdio.h>

#include "dns-tests.h"
#include "hash-tests.h"
#include "req-tests.h"
#include "resource-tests.h"
#include "utils-tests.h"

int
main() {
  printf("Testing hnsd...\n");

  test_dns();
  test_hash();
  test_req();
  test_resource();
  test_utils();

  printf("ok\n");

  return 0;
}
