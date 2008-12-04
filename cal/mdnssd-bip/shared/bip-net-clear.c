
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
}

