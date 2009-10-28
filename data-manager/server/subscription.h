
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include "bionet-data-manager.h"

typedef GHashTable bdm_peer_states_t;

typedef struct {
    int curr_seq; // -1 for unset. This means the subscription is in the pending state..
} bdm_subscription_t;

int bdm_subscriptions_remove_peer(bdm_peer_states_t *tbl, const char * peer_name);
int bdm_subscriptions_add_request(bdm_peer_states_t *tbl, const char * peer_name, const char * topic);

bdm_subscription_t * bdm_subscriptions_get(bdm_peer_states_t *tbl, const char * peer_name, const char * topic);
