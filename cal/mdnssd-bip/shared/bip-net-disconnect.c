
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <unistd.h>

#include "cal-mdnssd-bip.h"


void bip_net_disconnect(bip_peer_network_info_t *net) {
    if (net == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_net_disconnect: NULL net passed in");
        return;
    }

    bip_net_clear(net);

    if (NULL == net->socket_bio) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_net_disconnect: passed-in net does not have a connected socket");
        return;
    }

    BIO_free_all(net->socket_bio); // Free and close entire chain
    net->socket_bio = NULL;
    net->socket = -1;
    net->security_status = BIP_SEC_NOT_CONNECTED;
}

