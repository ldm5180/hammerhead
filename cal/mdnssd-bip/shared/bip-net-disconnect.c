
#include <unistd.h>

#include "cal-mdnssd-bip.h"


void bip_net_disconnect(bip_peer_network_info_t *net) {
    if (net == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_net_disconnect: NULL net passed in");
        return;
    }

    bip_net_clear(net);

    if (net->socket == -1) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_net_disconnect: passed-in net does not have a connected socket");
        return;
    }

    close(net->socket);
    net->socket = -1;
}

