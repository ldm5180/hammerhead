
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>
#include <string.h>

#include "cal-mdnssd-bip.h"


bip_peer_network_info_t *bip_net_new(const char *hostname, uint16_t port) {
    bip_peer_network_info_t *net;

    net = calloc(1, sizeof(bip_peer_network_info_t));
    if (net == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "bip_net_new: out of memory");
        return NULL;
    }

    if (hostname != NULL) {
        net->hostname = strdup(hostname);
        if (net->hostname == NULL) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "bip_net_new: out of memory");
            free(net);
            return NULL;
        }
    }

    net->port = port;

    net->socket = -1;  // not connected yet
    net->security_status = BIP_SEC_NOT_CONNECTED;
    // bio already NULL

    g_queue_init(&net->msg_send_queue);

    return net;
}

