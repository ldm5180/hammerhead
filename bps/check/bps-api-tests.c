#include "check-bps.h"

#include <check.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <stdio.h>
#include <unistd.h>

#include "bps/bps_socket.h"

static const char * msgs [] = {
    "0123456789",
    "9876543210",
    "Hello Computer"
};

static const int num_msgs = sizeof(msgs)/sizeof(msgs[0]);


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

START_TEST (check_msg_more) {
    int r;
    ssize_t bytes;

    int fd = bps_socket(0,0,0);
    fail_unless(fd >=0, "bps_socket returned error", strerror(errno));

    int fd_recv = bps_socket(0,0,0);
    fail_unless(fd >=0, "bps_socket returned error", strerror(errno));


    struct bps_sockaddr srcaddr, dstaddr;
    strncpy(srcaddr.uri, "ipn:1.1", BPS_EID_SIZE);
    strncpy(dstaddr.uri, "ipn:1.2", BPS_EID_SIZE);

    r = bps_bind(fd, &srcaddr, sizeof(struct bps_sockaddr));
    fail_unless(0==r, "bps_bind() failed", strerror(errno));

    r = bps_bind(fd_recv, &dstaddr, sizeof(struct bps_sockaddr));
    fail_unless(0==r, "bps_bind() failed", strerror(errno));

    bytes = bps_sendto(fd, (void*)msgs[0], strlen(msgs[0]), MSG_MORE,
            &dstaddr, sizeof(struct bps_sockaddr));
    fail_unless(bytes==strlen(msgs[0]), "bps_sendto() returned %d", bytes);

    bytes = bps_sendto(fd, (void*)msgs[1], strlen(msgs[1]), 0,
            &dstaddr, sizeof(struct bps_sockaddr));
    fail_unless(bytes==strlen(msgs[1]), "bps_sendto() returned %d", bytes);

    // Send part of next bundle, then receive first bundle, then send rest of second...
    bytes = bps_sendto(fd, (void*)msgs[0], strlen(msgs[0]), MSG_MORE,
            &dstaddr, sizeof(struct bps_sockaddr));
    fail_unless(bytes==strlen(msgs[0]), "bps_sendto() returned %d", bytes);


    char buf[64];
    bytes = bps_recv(fd_recv, buf, sizeof(buf), 0);

    fail_unless(bytes == strlen(msgs[0]) + strlen(msgs[1]),
            "Wrong size bundle received: %zd", bytes);

    fail_unless(0 == memcmp(msgs[0], buf, strlen(msgs[0])),
            "Bundle Corrupted");

    fail_unless(0 == memcmp(msgs[1], buf+strlen(msgs[0]), strlen(msgs[1])),
            "Bundle Corrupted");

    bytes = bps_sendto(fd, (void*)msgs[1], strlen(msgs[1]), 0,
            &dstaddr, sizeof(struct bps_sockaddr));
    fail_unless(bytes==strlen(msgs[1]), "bps_sendto() returned %d", bytes);

    r = bps_close(fd);
    fail_unless(0==r, "bps_close failed: %s", strerror(errno));

    bytes = bps_recv(fd_recv, buf, sizeof(buf), 0);

    fail_unless(bytes == strlen(msgs[0]) + strlen(msgs[1]),
            "Wrong size bundle received: %zd", bytes);

    fail_unless(0 == memcmp(msgs[0], buf, strlen(msgs[0])),
            "Bundle Corrupted");

    fail_unless(0 == memcmp(msgs[1], buf+strlen(msgs[0]), strlen(msgs[1])),
            "Bundle Corrupted");

    r = bps_close(fd_recv);
    fail_unless(0==r, "bps_close failed: %s", strerror(errno));


}
END_TEST

static void get_msg(int i, char str[64]) {
    int j, n;
    snprintf(str, 64, "%d ", i);

    n = i % 62;

    for(j=strlen(str); j<n; j++) {
        str[j] = 'X';
    }
    str[j] = '\0';
}

static void send_msg(int fd, int i, struct bps_sockaddr *dstaddr) {
    char msg[64];

    get_msg(i, msg);

    ssize_t bytes = bps_sendto(fd, msg, strlen(msg), 0,
            dstaddr, sizeof(struct bps_sockaddr));
    fail_unless(bytes==strlen(msg), "bps_sendto() returned %d", bytes);
}

static void check_accept_bdl(int fd, int i) {
    char msg[64];
    char buf[64];

    get_msg(i, msg);

    ssize_t bytes = bps_recv(fd, buf, sizeof(buf), 0);
    fail_unless(bytes == strlen(msg),
            "Wrong size bundle received: %zd, not %d", bytes, strlen(msg));

    buf[bytes] = '\0';
    fail_unless(0 == memcmp(msg, buf, strlen(msg)),
            "Bundle Corrupted ('%s' != '%s'", buf, msg);
    bytes = bps_recv(fd, buf, sizeof(buf), 0);
    fail_unless(0 == bytes, "Unexpected bytes in bundle");
    bps_close(fd);

    //fprintf(stderr, "Msg: '%s'\n", buf);
}

START_TEST (check_accept_queue) {
    int r;

    static const int SEND_BATCH=100;

    int bdl_fds[SEND_BATCH * 2];

    int fd = bps_socket(0,0,0);
    fail_unless(fd >=0, "bps_socket returned error", strerror(errno));

    int fd_recv = bps_socket(0,0,0);
    fail_unless(fd >=0, "bps_socket returned error", strerror(errno));


    struct bps_sockaddr srcaddr, dstaddr;
    strncpy(srcaddr.uri, "ipn:1.1", BPS_EID_SIZE);
    strncpy(dstaddr.uri, "ipn:1.2", BPS_EID_SIZE);

    r = bps_bind(fd, &srcaddr, sizeof(struct bps_sockaddr));
    fail_unless(0==r, "bps_bind() failed", strerror(errno));

    r = bps_bind(fd_recv, &dstaddr, sizeof(struct bps_sockaddr));
    fail_unless(0==r, "bps_bind() failed", strerror(errno));

    r = bps_listen(fd_recv, 3);
    fail_unless(0==r, "bps_listen() failed", strerror(errno));
    

    int si = 0; // Send index
    int ri = 0; // Receive index, bundle that have been accepted
    int pi = 0; // Processed index
    int i;

    // Send first batch of msgs
    for(; si<(SEND_BATCH); si++) {
        send_msg(fd, si, &dstaddr);
    }

    // Wait for first bundle
    {
        struct pollfd fds[] = {
            { fd_recv, POLLIN, 0 }
        };
        poll(fds, 1, 4000);
    }
    sleep(1); // Allow the sending thread to queue multiple bundles at once

    // Send second batch, and receive bundles as they arrive
    for(; si< 2*SEND_BATCH; si++) {

        struct pollfd fds[] = {
            { fd_recv, POLLIN, 0 }
        };

        send_msg(fd, si, &dstaddr);

        if( poll(fds, 1, 1) > 0 ) {
            if(fds[0].revents & POLLIN) {
                int bdl_fd = bps_accept(fd_recv, NULL, NULL);
                fail_unless(bdl_fd >= 0,
                        "Failed to accept bundle %d: %s", ri, strerror(errno));

                bdl_fds[ri++] = bdl_fd;
            }
        }

    }

    fail_unless(ri > 0, "No bundles accepted yet");

    // Receive rest of first batch
    for(i=ri; i<SEND_BATCH; i++) {
        int bdl_fd = bps_accept(fd_recv, NULL, NULL);
        fail_unless(bdl_fd >= 0,
                "Failed to accept bundle %d: %s", ri, strerror(errno));

        bdl_fds[ri++] = bdl_fd;
    }

    // Process  remaining first batch
    for(i=pi; i<ri; i++) {
        check_accept_bdl(bdl_fds[i], i);
        pi++;
    }

    // Receive all remaining
    for(i=ri; i<si; i++) {
        int bdl_fd = bps_accept(fd_recv, NULL, NULL);
        fail_unless(bdl_fd >= 0,
                "Failed to accept bundle %d: %s", ri, strerror(errno));

        bdl_fds[ri++] = bdl_fd;
    }
    // Process all remaining
    for(i=pi; i<ri; i++) {
        check_accept_bdl(bdl_fds[i], i);
        pi++;
    }
    fail_unless(ri == si, "Some bundles missing");

    // Cleanup
    r = bps_close(fd);
    fail_unless(0==r, "bps_close failed: %s", strerror(errno));

    r = bps_close(fd_recv);
    fail_unless(0==r, "bps_close failed: %s", strerror(errno));

}
END_TEST

void bps_api_tests_init(Suite *s) {
    TCase * tc = tcase_create ("bps-api");

    tcase_add_test (tc, check_bps_socket);
    tcase_add_test (tc, check_bps_send);
    tcase_add_test (tc, check_msg_more);
    tcase_add_test (tc, check_accept_queue);


    suite_add_tcase(s, tc);

}

