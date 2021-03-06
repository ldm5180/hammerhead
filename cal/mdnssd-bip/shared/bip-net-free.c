
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>

#include "cal-mdnssd-bip.h"


void bip_net_free(bip_peer_network_info_t *net) {
    if (net == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_net_free: NULL net passed in");
        return;
    }

    bip_net_clear(net);

    if (net->socket_bio != NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_net_free: BIO still open");
    }

    if (net->hostname != NULL) free(net->hostname);
    if (net->buffer != NULL) free(net->buffer);
    free(net);
}

