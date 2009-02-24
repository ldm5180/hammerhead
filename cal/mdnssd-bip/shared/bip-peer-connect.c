
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




int bip_peer_connect(const char *peer_name, bip_peer_t *peer) {
    int r;
    bip_peer_network_info_t *net;

    if (peer == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_peer_connect: NULL peer passed in");
        return -1;
    }

    net = bip_peer_get_connected_net(peer);
    if (net != NULL) return 0;

    while (peer->nets->len > 0) {
        net = g_ptr_array_index(peer->nets, 0);
        r = bip_net_connect(peer_name, net);
        if (r >= 0) return 0;
        g_ptr_array_remove_fast(peer->nets, net);
        bip_net_destroy(net);
    }

    return -1;
}

