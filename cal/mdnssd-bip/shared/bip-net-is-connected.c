
#include <unistd.h>

#include "cal-mdnssd-bip.h"


int bip_net_is_connected(bip_peer_network_info_t *net) {
    if (net == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_net_is_connected: NULL net passed in");
        return 0;
    }

    if (net->socket != -1) return 1;
    return 0;
}

