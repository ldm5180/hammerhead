
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>

#include "internal.h"
#include "bionet-util.h"


int bionet_node_matches_id(const bionet_node_t *node, const char *id) {
    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_matches_id: NULL node passed in!");
        return 0;
    }

    if (!bionet_name_component_matches(node->id, id)) return 0;

    return 1;
}


int bionet_node_matches_habtype_habid_nodeid(const bionet_node_t *node, const char *hab_type, const char *hab_id, const char *node_id) {
    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_matches_habtype_habid_nodeid: NULL node passed in!");
        return 0;
    }

    if (node->hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_matches_habtype_habid_nodeid: passed-in node has NULL hab!");
        return 0;
    }

    if (!bionet_name_component_matches(node->hab->type, hab_type)) return 0;
    if (!bionet_name_component_matches(node->hab->id, hab_id)) return 0;
    if (!bionet_name_component_matches(node->id, node_id)) return 0;

    return 1;
}

