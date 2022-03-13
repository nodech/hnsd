#include <stdio.h>
#include <req.h>

#include "test-util.h"

void
test_hsk_dns_req_create() {
  printf("  test_hsk_dns_req_create\n");

  const uint8_t data[] = {
     0x00, 0x7b, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x04, 0x68, 0x6e, 0x73,
     0x64, 0x09, 0x68, 0x61, 0x6e, 0x64, 0x73, 0x68,
     0x61, 0x6b, 0x65, 0x03, 0x6f, 0x72, 0x67, 0x00,
     0x00, 0x10, 0x00, 0x01
  };
  const struct sockaddr addr;

  hsk_dns_req_t *req = hsk_dns_req_create(data, sizeof(data), &addr);

  ASSERT_INT_EQ(req->id, 123);
  ASSERT_INT_EQ(memcmp(req->name, "\x04""hnsd""\x09""handshake""\x03""org", 20), 0);
  ASSERT_INT_EQ(req->type, 16); // TXT
  ASSERT_INT_EQ(req->class, 1); // IN

  free(req);
}


void
test_req() {
  printf("req.h tests...\n");

  test_hsk_dns_req_create();
}
