
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <glib.h>

#include "libbdm-internal.h"

extern GSList *bionet_habs;

static void cache_cleanup_hab(bionet_hab_t *hab) {
    GSList *i;

    //printf("hab %s ... ", bionet_hab_get_name(hab));

    // any hab subscriptions match (overlap)?
    for (i = libbdm_hab_subscriptions; i != NULL; i = i->next) {
        libbdm_hab_subscription_t *hab_sub = i->data;

        if (bionet_hab_matches_type_and_id(hab, hab_sub->hab_type, hab_sub->hab_id)) {
            //printf("matched hab subscription\n");
            return;
        }
    }

    // any matching node subscriptions?
    for (i = libbdm_node_subscriptions; i != NULL; i = i->next ) {
        libbdm_node_subscription_t *node_sub = i->data;

        if (bionet_hab_matches_type_and_id(hab, node_sub->hab_type, node_sub->hab_id)) {
            //printf("matched node subscription\n");
            return;
        }
    }

    // any matching datapoint subscriptions?
    for (i = libbdm_datapoint_subscriptions; i != NULL; i = i->next) {
        libbdm_datapoint_subscription_t *dp_sub = i->data;

        if (bionet_hab_matches_type_and_id(hab, dp_sub->hab_type, dp_sub->hab_id)) {
            //printf("matched datapoint subscription\n");
            return;
        }
    }

    // any matching stream subscriptions?
    for (i = libbdm_stream_subscriptions; i != NULL; i = i->next) {
        libbdm_datapoint_subscription_t *stream_sub = i->data;

        if (bionet_hab_matches_type_and_id(hab, stream_sub->hab_type, stream_sub->hab_id)) {
            //printf("matched stream subscription\n");
            return;
        }
    }
    
    //printf("successfully cleaned\n");

    // if we're here, then remove the hab from the client's view.
    // note that the hab_lost_message has already been sent to the appropriate
    libbdm_cache_remove_hab(hab);
}


void cache_cleanup_node(bionet_node_t *node) {
    GSList *i;

    //printf("node %s ... ", bionet_node_get_name(node));

    // any matching node subscriptions?
    for (i = libbdm_node_subscriptions; i != NULL; i = i->next ) {
        libbdm_node_subscription_t *node_sub = i->data;

        if (bionet_node_matches_habtype_habid_nodeid(node, node_sub->hab_type, node_sub->hab_id, node_sub->node_id)) {
            //printf("matching node subscription, return\n");
            return;
        }
    }

    // any matching datapoint subscriptions?
    for (i = libbdm_datapoint_subscriptions; i != NULL; i = i->next) {
        libbdm_datapoint_subscription_t *dp_sub = i->data;

        if (bionet_node_matches_habtype_habid_nodeid(node, dp_sub->hab_type, dp_sub->hab_id, dp_sub->node_id)) {
            //printf("matched datapoint subscription, return\n");
            return;
        }
    }

    // any matching stream subscriptions?
    for (i = libbdm_stream_subscriptions; i != NULL; i = i->next) {
        libbdm_datapoint_subscription_t *stream_sub = i->data;

        if (bionet_node_matches_habtype_habid_nodeid(node, stream_sub->hab_type, stream_sub->hab_id, stream_sub->node_id)) {
            //printf("matched stream subscription, return\n");
            return;
        }
    }

    //printf("successfully cleaned!\n");

    // if we're here, then nobody else cares about this node. remove it.
    libbdm_cache_remove_node(node);
    bionet_node_free(node);
}


void libbdm_cache_cleanup_habs() {
    GSList *i;

    for (i = bionet_habs; i != NULL; i = i->next) {
        bionet_hab_t *hab = i->data;

        if (hab == NULL)
            continue;

        cache_cleanup_hab(hab);
    }
}


void libbdm_cache_cleanup_nodes() {
    GSList *i;

    for (i = bionet_habs; i != NULL; i = i->next) {
        bionet_hab_t *hab = i->data;
        int j;

        if (hab == NULL)
            continue;

        j = bionet_hab_get_num_nodes(hab);
        while (j > 0) {
            j--;
            bionet_node_t *node = bionet_hab_get_node_by_index(hab, j);

            if (node == NULL) {
                continue;
            }

            cache_cleanup_node(node);
        }

        cache_cleanup_hab(hab);
    }
}


