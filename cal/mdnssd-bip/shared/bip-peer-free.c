
#include <stdlib.h>

#include <glib.h>

#include "cal-mdnssd-bip.h"


void bip_peer_free(bip_peer_t *peer) {
    if (peer == NULL) return;
    if (peer->net != NULL) bip_net_free(peer->net);
    while (g_slist_length(peer->subscriptions) > 0) {
        char *s = g_slist_nth_data(peer->subscriptions, 0);
        peer->subscriptions = g_slist_remove(peer->subscriptions, s);
        free(s);
    }
    free(peer);
}

