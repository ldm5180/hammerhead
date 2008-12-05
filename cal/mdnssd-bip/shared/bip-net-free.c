
#include <stdlib.h>

#include "cal-mdnssd-bip.h"


void bip_net_free(bip_peer_network_info_t *net) {
    if (net == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_net_free: NULL net passed in");
        return;
    }

    bip_net_clear(net);

    if (net->hostname != NULL) free(net->hostname);
    if (net->buffer != NULL) free(net->buffer);
    free(net);
}

