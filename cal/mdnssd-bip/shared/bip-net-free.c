
#include <stdlib.h>

#include "cal-mdnssd-bip.h"


void bip_net_free(bip_peer_network_info_t *net) {
    if (net == NULL) return;
    if (net->hostname != NULL) free(net->hostname);
    if (net->buffer != NULL) free(net->buffer);
    free(net);
}

