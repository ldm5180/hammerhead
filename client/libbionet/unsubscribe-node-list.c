
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <glib.h>

#include "bionet.h"
#include "libbionet-internal.h"


extern GSList *bionet_habs;

int bionet_unsubscribe_node_list_by_habtype_habid_nodeid(const char *hab_type,  const char *hab_id, const char *node_id) {
    int r;
    char publisher[BIONET_NAME_COMPONENT_MAX_LEN * 2];
    char topic[BIONET_NAME_COMPONENT_MAX_LEN + 2];  // the +2 is for the leading "N " subscription family specifier
    GSList *i = libbionet_node_subscriptions;

    while (i != NULL) {
        libbionet_node_subscription_t *node_sub = i->data;

        if (node_sub == NULL) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_unsubscribe_node_list...(): NULL node subscription!");
            i = i->next;
            continue;
        }

        if ((node_sub->hab_type == NULL) || 
            (node_sub->hab_id == NULL) ||
            (node_sub->node_id == NULL)) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_unsubscribe_node_list...(): NULL node subscription component!");
            i = i->next;
            continue;
        }

        // if the subscription is not an exact match, skip it
        if ((strcmp(node_sub->hab_type, hab_type) != 0) ||
            (strcmp(node_sub->hab_id, hab_id) != 0) ||
            (strcmp(node_sub->node_id, node_id) != 0)) {
            i = i->next;
            continue;
        }

        free(node_sub->hab_type);
        free(node_sub->hab_id);
        free(node_sub->node_id);
        free(node_sub);
        
        i->data = NULL;
        libbionet_node_subscriptions = g_slist_delete_link(libbionet_node_subscriptions, i);


        if (libbionet_callback_lost_node != NULL) {
            GSList *h;

            // need to walk over the entire hab/node list to unsubscribe from each :(
            for (h = bionet_habs; h != NULL; h = h->next) {
                bionet_hab_t *hab = h->data;
                int j;

                for (j = 0; j < bionet_hab_get_num_nodes(hab); j++) {
                    bionet_node_t *node = bionet_hab_get_node_by_index(hab, j);
                    int found_matching_sub = 0;
                    GSList *k;

                    // walk through all the node_subscriptions: if none match, send the lost_node callback
                    for (k = libbionet_node_subscriptions; k != NULL; k = k->next) {
                        libbionet_node_subscription_t *sub = k->data;
                        if (bionet_node_matches_habtype_habid_nodeid(
                            node, sub->hab_type, sub->hab_id, sub->node_id)) {
                            found_matching_sub = 1;
                            break;
                        }
                    }

                    if ( !found_matching_sub )
                        libbionet_callback_lost_node(node);
                }
            }
        }

        libbionet_cache_cleanup_nodes();

        r = snprintf(publisher, sizeof(publisher), "%s.%s", hab_type, hab_id);
        if (r >= sizeof(publisher)) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_unsubscribe_node_list_by_habtype_habid_nodeid(): HAB name '%s.%s' too long", hab_type, hab_id);
            return -1;
        }

        r = snprintf(topic, sizeof(topic), "N %s", node_id);
        if (r >= sizeof(topic)) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_unsubscribe_node_list_by_habtype_habid_nodeid(): Node name '%s' too long", node_id);
            return -1;
        }

        // send the node unsubscription request to to all matching nodes
        r = cal_client.unsubscribe(publisher, topic);
        if (!r) return -1;

        return 0;
    }

    errno = ENOENT;
    return -1;
}




int bionet_unsubscribe_node_list_by_name(const char *node_name) {
    char *hab_type;
    char *hab_id;
    char *node_id;
    int r;

    r = bionet_split_node_name(node_name, &hab_type, &hab_id, &node_id);
    if (r != 0) {
        // a helpful error message has already been logged
        errno = EINVAL;
        return -1;
    }

    return bionet_unsubscribe_node_list_by_habtype_habid_nodeid(hab_type, hab_id, node_id);
}

