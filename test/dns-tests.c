#include <stdio.h>
#include <dns.h>
#include <bio.h>

#include "test-util.h"
#include "data/record_read_vectors.h"

void
test_hsk_dns_label_split() {
  printf("  test_hsk_dns_label_split\n");

  const uint8_t name1[] = "\x04""hnsd""\x09""handshake""\x03""org""\x00";

  int count1 = hsk_dns_label_count((uint8_t *)&name1);
  ASSERT_INT_EQ(count1, 3);

  uint8_t labels[count1];
  int count2 = hsk_dns_label_split((uint8_t *)&name1, (uint8_t *)&labels, count1);
  ASSERT_INT_EQ(count2, 3);
  ASSERT_INT_EQ(labels[0], 0x00);
  ASSERT_INT_EQ(labels[1], 0x05);
  ASSERT_INT_EQ(labels[2], 0x0f);

  uint8_t ret1[HSK_DNS_MAX_LABEL + 2];
  uint8_t ret2[HSK_DNS_MAX_LABEL + 2];
  uint8_t ret3[HSK_DNS_MAX_LABEL + 2];
  uint8_t ret4[HSK_DNS_MAX_LABEL + 2];
  // Fill arrays with 0xff to ensure no extra 0x00 is written
  memset(&ret1, 0xff, sizeof(ret1));
  memset(&ret2, 0xff, sizeof(ret2));
  memset(&ret3, 0xff, sizeof(ret3));
  memset(&ret4, 0xff, sizeof(ret4));
  int label1 = hsk_dns_label_get((uint8_t *)&name1, 0, ret1);
  int label2 = hsk_dns_label_get((uint8_t *)&name1, 1, ret2);
  int label3 = hsk_dns_label_get((uint8_t *)&name1, 2, ret3);
  int label4 = hsk_dns_label_get((uint8_t *)&name1, -1, ret4);
  ASSERT_INT_EQ(label1, 4);
  ASSERT_INT_EQ(label2, 9);
  ASSERT_INT_EQ(label3, 3);
  ASSERT_INT_EQ(label4, 3);
  // label_get writes length byte, label, and null terminator but
  // only returns the length of the label.
  // Compare one extra byte to ensure the 0xff from array init is still there.
  ASSERT_INT_EQ(memcmp(&ret1, "\x04""hnsd""\x00\xff", label1 + 3), 0);
  ASSERT_INT_EQ(memcmp(&ret2, "\x09""handshake""\x00\xff", label2 + 3), 0);
  ASSERT_INT_EQ(memcmp(&ret3, "\x03""org""\x00\xff", label3 + 3), 0);
  ASSERT_INT_EQ(memcmp(&ret4, "\x03""org""\x00\xff", label4 + 3), 0);

  uint8_t ret5[HSK_DNS_MAX_NAME];
  uint8_t ret6[HSK_DNS_MAX_NAME];
  uint8_t ret7[HSK_DNS_MAX_NAME];
  uint8_t ret8[HSK_DNS_MAX_NAME];
  memset(&ret5, 0xff, sizeof(ret5));
  memset(&ret6, 0xff, sizeof(ret6));
  memset(&ret7, 0xff, sizeof(ret7));
  memset(&ret8, 0xff, sizeof(ret8));
  int label5 = hsk_dns_label_from((uint8_t *)&name1, 0, ret5);
  int label6 = hsk_dns_label_from((uint8_t *)&name1, 1, ret6);
  int label7 = hsk_dns_label_from((uint8_t *)&name1, 2, ret7);
  int label8 = hsk_dns_label_from((uint8_t *)&name1, -1, ret8);
  ASSERT_INT_EQ(label5, 20);
  ASSERT_INT_EQ(label6, 15);
  ASSERT_INT_EQ(label7, 5);
  ASSERT_INT_EQ(label8, 5);
  // label_from writes length byte, label and null terminator and
  // returns the entire length of data written.
  // Compare one extra byte to ensure the 0xff from array init is still there.
  ASSERT_INT_EQ(memcmp(&ret5, "\x04""hnsd""\x09""handshake""\x03""org""\x00\xff", label5 + 1), 0);
  ASSERT_INT_EQ(memcmp(&ret6, "\x09""handshake""\x03""org""\x00\xff", label6 + 1), 0);
  ASSERT_INT_EQ(memcmp(&ret7, "\x03""org""\x00\xff", label7 + 1), 0);
  ASSERT_INT_EQ(memcmp(&ret8, "\x03""org""\x00\xff", label8 + 1), 0);
}

void
test_hsk_dns_msg_read() {
  printf("  test_dns_msg_read\n");

  for (int i = 0; i < ARRAY_SIZE(record_read_vectors); i++) {
    record_read_vector_t record_read_vector = record_read_vectors[i];
    printf("   TYPE:%02d %s\n", record_read_vector.type, record_read_vector.name1);

    // Build DNS message from test vector
    size_t meta_len = 2 + 2 + 4 + 2; // type, class, ttl, rd size
    size_t rd_len = record_read_vector.data_len;
    size_t total_len = record_read_msg_len + meta_len + rd_len;
    uint8_t data[total_len];
    uint8_t *data_ = (uint8_t *)&data;

    write_bytes(&data_, record_read_msg, record_read_msg_len);
    write_u16be(&data_, record_read_vector.type); // type
    write_u16be(&data_, HSK_DNS_IN);              // class
    write_u32be(&data_, 0x00);                    // ttl
    write_u16be(&data_, rd_len);                  // rd size
    write_bytes(&data_, (uint8_t *)record_read_vector.data, rd_len);

    // Read
    hsk_dns_msg_t *msg = hsk_dns_msg_alloc();
    data_ = (uint8_t *)&data;
    bool success = hsk_dns_msg_read(&data_, &total_len, msg);
    ASSERT_BOOL_EQ(success, record_read_vector.valid);

    if (!success)
      goto done;

    // Grab first answer
    hsk_dns_rr_t *rr = msg->an.items[0];

    // Read names, convert to presentation format, check
    switch (record_read_vector.type) {
      case HSK_DNS_NS: {
        hsk_dns_ns_rd_t *r = (hsk_dns_ns_rd_t *)rr->rd;
        char name1[HSK_DNS_MAX_NAME_STRING] = {0};
        ASSERT(hsk_dns_name_to_string(r->ns, name1));
        ASSERT_INT_EQ(strcmp(name1, record_read_vector.name1), 0);
        break;
      }
      case HSK_DNS_CNAME: {
        hsk_dns_cname_rd_t *r = (hsk_dns_cname_rd_t *)rr->rd;
        char name1[HSK_DNS_MAX_NAME_STRING] = {0};
        ASSERT(hsk_dns_name_to_string(r->target, name1));
        ASSERT_INT_EQ(strcmp(name1, record_read_vector.name1), 0);
        break;
      }
      case HSK_DNS_SOA: {
        hsk_dns_soa_rd_t *r = (hsk_dns_soa_rd_t *)rr->rd;
        char name1[HSK_DNS_MAX_NAME_STRING] = {0};
        ASSERT(hsk_dns_name_to_string(r->ns, name1));
        ASSERT_INT_EQ(strcmp(name1, record_read_vector.name1), 0);
        char name2[HSK_DNS_MAX_NAME_STRING] = {0};
        ASSERT(hsk_dns_name_to_string(r->mbox, name2));
        ASSERT_INT_EQ(strcmp(name2, record_read_vector.name2), 0);
        break;
      }
      case HSK_DNS_PTR: {
        hsk_dns_ptr_rd_t *r = (hsk_dns_ptr_rd_t *)rr->rd;
        char name1[HSK_DNS_MAX_NAME_STRING] = {0};
        ASSERT(hsk_dns_name_to_string(r->ptr, name1));
        ASSERT_INT_EQ(strcmp(name1, record_read_vector.name1), 0);
        break;
      }
      case HSK_DNS_MX: {
        hsk_dns_mx_rd_t *r = (hsk_dns_mx_rd_t *)rr->rd;
        char name1[HSK_DNS_MAX_NAME_STRING] = {0};
        ASSERT(hsk_dns_name_to_string(r->mx, name1));
        ASSERT_INT_EQ(strcmp(name1, record_read_vector.name1), 0);
        break;
      }
      case HSK_DNS_RP: {
        hsk_dns_rp_rd_t *r = (hsk_dns_rp_rd_t *)rr->rd;
        char name1[HSK_DNS_MAX_NAME_STRING] = {0};
        ASSERT(hsk_dns_name_to_string(r->mbox, name1));
        ASSERT_INT_EQ(strcmp(name1, record_read_vector.name1), 0);
        char name2[HSK_DNS_MAX_NAME_STRING] = {0};
        ASSERT(hsk_dns_name_to_string(r->txt, name2));
        ASSERT_INT_EQ(strcmp(name2, record_read_vector.name2), 0);
        break;
      }
      case HSK_DNS_SRV: {
        hsk_dns_srv_rd_t *r = (hsk_dns_srv_rd_t *)rr->rd;
        char name1[HSK_DNS_MAX_NAME_STRING] = {0};
        ASSERT(hsk_dns_name_to_string(r->target, name1));
        ASSERT_INT_EQ(strcmp(name1, record_read_vector.name1), 0);
        break;
      }
      case HSK_DNS_RRSIG: {
        hsk_dns_rrsig_rd_t *r = (hsk_dns_rrsig_rd_t *)rr->rd;
        char name1[HSK_DNS_MAX_NAME_STRING] = {0};
        ASSERT(hsk_dns_name_to_string(r->signer_name, name1));
        ASSERT_INT_EQ(strcmp(name1, record_read_vector.name1), 0);
        break;
      }
      case HSK_DNS_NSEC: {
        hsk_dns_nsec_rd_t *r = (hsk_dns_nsec_rd_t *)rr->rd;
        char name1[HSK_DNS_MAX_NAME_STRING] = {0};
        ASSERT(hsk_dns_name_to_string(r->next_domain, name1));
        ASSERT_INT_EQ(strcmp(name1, record_read_vector.name1), 0);
        break;
      }
    }

done:
    hsk_dns_msg_free(msg);
  }
}

void
test_hsk_dns_msg_write() {
  printf("  test_hsk_dns_msg_write\n");
  for (int i = 0; i < ARRAY_SIZE(record_read_vectors); i++) {
    record_read_vector_t record_read_vector = record_read_vectors[i];

    // Build expected DNS message from test vector
    size_t meta_len = 2 + 2 + 4 + 2; // type, class, ttl, rd size
    size_t rd_len = record_read_vector.data_len;
    size_t total_len = record_read_msg_len + meta_len + rd_len;
    uint8_t data[total_len];
    uint8_t *data_ = (uint8_t *)&data;

    write_bytes(&data_, record_read_msg, record_read_msg_len);
    write_u16be(&data_, record_read_vector.type); // type
    write_u16be(&data_, HSK_DNS_IN);              // class
    write_u32be(&data_, 0x00);                    // ttl
    write_u16be(&data_, rd_len);                  // rd size
    write_bytes(&data_, (uint8_t *)record_read_vector.data, rd_len);

    // Build msg to serialize
    hsk_dns_msg_t *msg = hsk_dns_msg_alloc();
    hsk_dns_rr_t *rr = hsk_dns_rr_alloc();
    hsk_dns_name_from_string(record_read_msg_qname, rr->name);
    rr->type = record_read_vector.type;
    rr->class = HSK_DNS_IN;
    rr->ttl = 0;

    if (record_read_vector.valid) {
      rr->rd = hsk_dns_rd_alloc(record_read_vector.type);
    } else {
      rr->rd = malloc(sizeof(hsk_dns_invalid_cname_rd_t));
    }

    hsk_dns_rrs_push(&msg->an, rr);

    // Write names, convert from presentation format
    switch (record_read_vector.type) {
      case HSK_DNS_SOA:
      case HSK_DNS_MX:
      case HSK_DNS_SRV:
      case HSK_DNS_RRSIG:
      case HSK_DNS_NSEC:
        // Only testing simple records with just names
        hsk_dns_msg_free(msg);
        continue;
      case HSK_DNS_NS: {
        hsk_dns_ns_rd_t *ns = rr->rd;
        hsk_dns_name_from_string(record_read_vector.name1, ns->ns);
        break;
      }
      case HSK_DNS_CNAME: {
        hsk_dns_cname_rd_t *cname = rr->rd;
        if (record_read_vector.valid) {
          hsk_dns_name_from_string(record_read_vector.name1, cname->target);
        } else {
          memcpy(cname->target, record_read_vector.invalidname, record_read_vector.invalidname_len);
        }
        break;
      }
      case HSK_DNS_PTR: {
        hsk_dns_ptr_rd_t *ptr = rr->rd;
        hsk_dns_name_from_string(record_read_vector.name1, ptr->ptr);
        break;
      }
      case HSK_DNS_RP: {
        hsk_dns_rp_rd_t *rp = rr->rd;
        hsk_dns_name_from_string(record_read_vector.name1, rp->mbox);
        hsk_dns_name_from_string(record_read_vector.name2, rp->txt);
        break;
      }
    }

    printf("   TYPE:%02d %s\n",record_read_vector.type, record_read_vector.name1);

    uint8_t actual[1024]; // enough for invalid message
    uint8_t *actual_ = (uint8_t *)&actual;
    int written = hsk_dns_msg_write(msg, &actual_);

    if (record_read_vector.valid) {
      ASSERT_INT_EQ(written, total_len);
      ASSERT_INT_EQ(memcmp(&data, &actual, total_len), 0);
    } else {
      ASSERT_INT_NEQ(written, total_len);
      ASSERT_INT_NEQ(memcmp(&data, &actual, total_len), 0);
    }

    // Also frees rr and rd
    hsk_dns_msg_free(msg);
  }
}

void test_dns() {
  printf("dns.h tests...\n");

  test_hsk_dns_label_split();
  test_hsk_dns_msg_read();
  test_hsk_dns_msg_write();
}
