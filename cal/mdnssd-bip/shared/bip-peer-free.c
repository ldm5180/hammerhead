
#include <stdlib.h>

#include <glib.h>

#include "cal-mdnssd-bip.h"


void bip_peer_free(bip_peer_t *peer) {
    if (peer == NULL) return;

    while (peer->nets->len > 0) {
        bip_peer_network_info_t *net = g_ptr_array_remove_index_fast(peer->nets, 0);
        bip_net_destroy(net);
    }

    g_ptr_array_free(peer->nets, TRUE);

    while (g_slist_length(peer->subscriptions) > 0) {
        char *s = g_slist_nth_data(peer->subscriptions, 0);
        peer->subscriptions = g_slist_remove(peer->subscriptions, s);
        free(s);
    }

    free(peer);
}

