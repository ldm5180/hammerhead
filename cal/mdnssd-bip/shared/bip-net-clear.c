
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>

#include "cal-mdnssd-bip.h"


void bip_net_clear(bip_peer_network_info_t *net) {
    if (net == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_net_clear: NULL net passed in");
        return;
    }

    net->header_index = 0;

    if (net->buffer != NULL) {
        free(net->buffer);
        net->buffer = NULL;
    }
    
    bip_buf_t * buf;
    while( (buf = g_queue_pop_head(&net->msg_send_queue))) {
        free(buf->data);
        free(buf);
    }
}

