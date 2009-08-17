
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <glib.h>
#include <dns_sd.h>

#include "cal-mdnssd-bip.h"


// Returns fd of connection if its in-progress, -1 if there are no more nets
int bip_peer_connect_nonblock(bip_peer_t * peer) {
    if (peer == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_peer_connect: NULL peer passed in");
        return -1;
    }

    while (peer->nets->len > 0) {
        bip_peer_network_info_t *net = g_ptr_array_index(peer->nets, 0);
        int fd = bip_net_connect_nonblock(peer->peer_name, net);
        if ( fd >= 0 ) {
            return fd;
        }

        g_ptr_array_remove_fast(peer->nets, net);
        bip_net_destroy(net);
    }

    return -1;
}


//
// Finish the connect procedure. Call this when the peer->net
// connect socket is writable:
// 
// If there was an error, then the failed net will be removed from the peer. Its up to the caller to try the next one
//
// @return bio, if connected. NULL on error
int bip_peer_connect_finish(bip_peer_t * peer) {
    int r = -1;

    if ( peer->nets->len > 0 ) {
        bip_peer_network_info_t *net = g_ptr_array_index(peer->nets, 0);
        r = bip_net_connect_check(peer->peer_name, net);
        if(r < 0) {
            g_ptr_array_remove_fast(peer->nets, net);
            bip_net_destroy(net);
        }
    }

    return r;
}
