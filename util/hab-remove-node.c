
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>
#include <errno.h>

#include "internal.h"
#include "bionet-util.h"


bionet_node_t * bionet_hab_remove_node_by_id(bionet_hab_t *hab, const char *node_id) {
    int i;


    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_remove_node_by_id(): NULL HAB passed in");
	errno = EINVAL;
        return NULL;
    }

    if (node_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_remove_node_by_id(): NULL Node-ID passed in");
	errno = EINVAL;
        return NULL;
    }

    for (i = 0; i < g_slist_length(hab->nodes); i ++) {
        bionet_node_t *node;

        node = g_slist_nth_data(hab->nodes, i);
        if (strcmp(node->id, node_id) == 0) {
            hab->nodes = g_slist_remove(hab->nodes, node);
            return node;
        }
    }

    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_remove_node_by_id(): Node '%s' not found in HAB", node_id);
    return NULL;
}


int bionet_hab_remove_all_nodes(bionet_hab_t *hab) {
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_remove_all_nodes(): NULL HAB passed in");
        return -1;
    }


    // remove all of this HAB's Nodes
    do {
        bionet_node_t *node;

        node = g_slist_nth_data(hab->nodes, 0);
        if (node == NULL) break;  // done

        hab->nodes = g_slist_remove(hab->nodes, node);

        bionet_node_free(node);
    } while(1);

    return 0;
}


