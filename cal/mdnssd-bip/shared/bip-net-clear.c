
#include <stdlib.h>

#include "cal-mdnssd-bip.h"


void bip_net_clear(bip_peer_network_info_t *net) {
    net->header_index = 0;

    if (net->buffer != NULL) {
        free(net->buffer);
        net->buffer = NULL;
    }
}

