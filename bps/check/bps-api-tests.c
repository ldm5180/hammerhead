#include "check-bps.h"

#include <check.h>
#include <errno.h>
#include <string.h>

#include "bps/bps_socket.h"


START_TEST (check_bps_socket) {
    int r;
    int fd = bps_socket(0,0,0);

    // While 0, 1, and 2 are technically valid, they should 
    // be already taken by stdio
    fail_unless(fd >=0, "bps_socket returned error", strerror(errno));
    fail_unless(fd > 2, "bps_socket returned socket in use");


    struct bps_sockaddr srcaddr;
    strncpy(srcaddr.uri, "ipn:1.1", BPS_EID_SIZE);
    r = bps_bind(fd, &srcaddr, sizeof(struct bps_sockaddr));
    fail_unless(0==r, "bps_bind() failed", strerror(errno));

    r = bps_close(fd);
    fail_unless(0==r, "bps_close failed: %s", strerror(errno));

}
END_TEST


START_TEST (check_bps_send) {
    int r;
    ssize_t bytes;

    int fd = bps_socket(0,0,0);
    fail_unless(fd >=0, "bps_socket returned error", strerror(errno));


    struct bps_sockaddr srcaddr, dstaddr;
    strncpy(srcaddr.uri, "ipn:1.1", BPS_EID_SIZE);
    strncpy(dstaddr.uri, "ipn:1.2", BPS_EID_SIZE);

    r = bps_bind(fd, &srcaddr, sizeof(struct bps_sockaddr));
    fail_unless(0==r, "bps_bind() failed", strerror(errno));

    bytes = bps_sendto(fd, "0123456789", 11, MSG_MORE,
            &srcaddr, sizeof(struct bps_sockaddr));
    fail_unless(bytes==11, "bps_sendto() returned %d", bytes);

    bytes = bps_sendto(fd, "0123456789", 11, 0,
            &srcaddr, sizeof(struct bps_sockaddr));
    fail_unless(bytes==11, "bps_sendto() returned %d", bytes);


    r = bps_close(fd);
    fail_unless(0==r, "bps_close failed: %s", strerror(errno));

}
END_TEST

void bps_api_tests_init(Suite *s) {
    TCase * tc = tcase_create ("bps-api");

    tcase_add_test (tc, check_bps_socket);
    tcase_add_test (tc, check_bps_send);

    suite_add_tcase(s, tc);

}

