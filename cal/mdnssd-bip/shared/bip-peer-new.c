
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>

#include "cal-mdnssd-bip.h"


bip_peer_t *bip_peer_new(void) {
    bip_peer_t *peer;

    peer = malloc(sizeof(bip_peer_t));
    if (peer == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "bip_peer_new: out of memory");
        return NULL;
    };

    peer->nets = g_ptr_array_new();
    if (peer->nets == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "bip_peer_new: out of memory");
        free(peer);
        return NULL;
    };

    peer->subscriptions = NULL;

    return peer;
}

