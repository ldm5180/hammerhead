
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#ifndef BPS_SOCK_TABLE_H
#define BPS_SOCK_TABLE_H

#include "bps_socket.h"
#include "bps-private.h"

int bps_sock_table_add(bps_socket_t * sock);

// Create and add new socket
bps_socket_t * bps_sock_new(void);

// Lookup socket by user's sockfd
bps_socket_t * bps_sock_table_lookup_fd(int fd);

// Remove and return the sock for this fd
bps_socket_t * bps_sock_table_remove_fd(int fd);


#endif  //BPS_SOCK_TABLE_H

