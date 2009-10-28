
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include "bionet-data-manager.h"
#include "subscription.h"
#include <glib.h>

bdm_peer_states_t * bdm_subscriptions_new() {
    return (bdm_peer_states_t*)g_hash_table_new_full(
            NULL, NULL, free, (GDestroyNotify)g_hash_table_destroy);
}

int bdm_subscriptions_add_request(
        bdm_peer_states_t *tbl,
        const char * peer_name,
        const char * topic)
{

    bdm_subscription_t * sub;
    GHashTable * peer = (GHashTable*)g_hash_table_lookup(tbl, peer_name); 
    if(peer) {
        sub = (bdm_subscription_t*)g_hash_table_lookup(peer, topic); 
    } else {
        peer = g_hash_table_new_full(NULL, NULL, free, free);
        if (NULL == peer) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s: Out of memory!", __FUNCTION__);
            return -1;
        }
        g_hash_table_insert(tbl, strdup(peer_name), peer); 

        sub = NULL;
    }

    if (sub) {
        // Already there
        return 0;
    }

    sub = malloc(sizeof(bdm_subscription_t));
    if (NULL == sub) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s: Out of memory!", __FUNCTION__);
        return -1;
    }

    sub->curr_seq = -1;
    g_hash_table_insert(peer, strdup(topic), sub); 

    return 0;
}


int bdm_subscriptions_remove_peer(bdm_peer_states_t *tbl, const char * peer_name)
{
    if( g_hash_table_remove(tbl, peer_name) ){
        return 0;
    } else {
        return -1;
    }
}


bdm_subscription_t * bdm_subscriptions_get(
        bdm_peer_states_t *tbl,
        const char * peer_name,
        const char * topic)
{
    bdm_subscription_t * sub;
    GHashTable * peer = (GHashTable*)g_hash_table_lookup(tbl, peer_name); 
    if( NULL == peer) {
        return NULL;
    }

    sub = (bdm_subscription_t*)g_hash_table_lookup(peer, topic); 
    return sub;
}



