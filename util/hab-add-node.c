
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <glib.h>

#include "internal.h"
#include "bionet-util.h"


int bionet_hab_add_node(bionet_hab_t *hab, const bionet_node_t *node) {
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_add_node(): NULL HAB passed in");
        return -1;
    }

    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_add_node(): NULL Node passed in");
        return -1;
    }

    if (bionet_hab_get_node_by_id(hab, node->id) != NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_add_node(): Node %s already present in HAB %s.%s", node->id, hab->type, hab->id);
        return -1;
    }

    // ok, add the node to the hab's node-list
    hab->nodes = g_slist_append(hab->nodes, (gpointer)node);
    return 0;
}


