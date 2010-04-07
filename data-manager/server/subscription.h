
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#ifndef SUBSCRIPTION_H
#define SUBSCRIPTION_H

#include <glib.h>

typedef GHashTable bdm_peer_states_t;

typedef struct {
    int curr_seq; // -1 for unset. This means the subscription is in the pending state..
} bdm_subscription_t;

bdm_peer_states_t * bdm_subscriptions_new(void);
int bdm_subscriptions_remove_peer(bdm_peer_states_t *tbl, const char * peer_name);
int bdm_subscriptions_add(bdm_peer_states_t *tbl, const char * peer_name, const char * topic, bdm_subscription_t * sub);
int bdm_subscriptions_add_request(bdm_peer_states_t *tbl, const char * peer_name, const char * topic);

bdm_subscription_t * bdm_subscriptions_get(bdm_peer_states_t *tbl, const char * peer_name, const char * topic);

#endif
