
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bip-socket-api.h"

#include <glib.h>

#include "cal-mdnssd-bip.h"




bip_peer_network_info_t *bip_peer_get_connected_net(const bip_peer_t *peer) {
    int i;

    if (peer == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_peer_get_connected_net: NULL peer passed in");
        return NULL;
    }

    for (i = 0; i < peer->nets->len; i ++) {
        bip_peer_network_info_t *net;
        net = g_ptr_array_index(peer->nets, i);
        if (net->socket_bio != NULL) return net;;
    }

    return NULL;
}

