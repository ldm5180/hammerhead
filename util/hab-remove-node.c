
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <string.h>

#include "bionet-util.h"




int bionet_hab_remove_node_by_id(bionet_hab_t *hab, const char *node_id) {
    int i;


    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_remove_node_by_id(): NULL HAB passed in");
        return -1;
    }

    if (node_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_remove_node_by_id(): NULL Node-ID passed in");
        return -1;
    }

    for (i = 0; i < g_slist_length(hab->nodes); i ++) {
        bionet_node_t *node;

        node = g_slist_nth_data(hab->nodes, i);
        if (strcmp(node->id, node_id) == 0) {
            hab->nodes = g_slist_remove(hab->nodes, node);
            return 0;
        }
    }

    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_remove_node_by_id(): Node '%s' not found in HAB", node_id);
    return -1;
}

