
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include "bps-sock-table.h"

static bps_socket_t ** sock_table = NULL;
static unsigned int num_socks = 0;


static int sock_cmp(const void *v_a, const void *v_b)
{
    bps_socket_t * a = *((bps_socket_t**)v_a);
    bps_socket_t * b = *((bps_socket_t**)v_b);

    return (a->usrfd - b->usrfd);
}

int bps_sock_table_add(bps_socket_t * sock) {
    bps_socket_t ** tmp_sock_table;

    num_socks++;
    tmp_sock_table = realloc(sock_table, num_socks * sizeof(bps_socket_t*));
    if ( tmp_sock_table == NULL ) return -1;

    sock_table = tmp_sock_table;
    sock_table[num_socks-1] = sock;

    qsort(sock_table, num_socks, sizeof(bps_socket_t*), sock_cmp);

    return 0;
}

bps_socket_t * bps_sock_table_lookup_fd(int fd) {

    bps_socket_t key_sock;
    key_sock.usrfd = fd;
    bps_socket_t * key = &key_sock;

    bps_socket_t ** sock = bsearch(&key, (void *)sock_table, 
            num_socks, sizeof(bps_socket_t*), sock_cmp);

    return *sock;
}

bps_socket_t * bps_sock_new(void) {
    int r;

    bps_socket_t * new_sock = NULL;

    // Create new accept object
    new_sock = calloc(1, sizeof(bps_socket_t));
    if ( new_sock == NULL ) return NULL;

    new_sock->opts.bundleLifetime = BPS_DEFAULT_TTL;
    new_sock->opts.classOfService = BPS_DEFAULT_CLASS_OF_SERVICE;
    new_sock->opts.custodySwitch  = BPS_DEFAULT_CUSTODY_SWITCH;

    int fds[2];
    r = socketpair(AF_UNIX, SOCK_DGRAM, 0, fds);
    if ( r != 0 ) goto cleanup;

    new_sock->libfd = fds[0];
    new_sock->usrfd = fds[1];

    // Save connection pointer into fd lookup table
    if( bps_sock_table_add(new_sock) ) goto cleanup;

    return new_sock;

cleanup:
    {
        int errno_save = errno;
        if(new_sock) {
            if(new_sock->libfd >= 0) {
                close(new_sock->libfd);
            }
            if(new_sock->usrfd >= 0) {
                close(new_sock->usrfd);
            }

            free(new_sock);
        }

        errno = errno_save;
    }

    return NULL;

}

