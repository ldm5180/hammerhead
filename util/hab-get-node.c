
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <string.h>

#include "internal.h"
#include "bionet-util.h"


bionet_node_t *bionet_hab_get_node_by_id(bionet_hab_t *hab, const char *node_id) {
    int i;


    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_get_node_by_id(): NULL HAB passed in");
        return NULL;
    }

    if (node_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_get_node_by_id(): NULL Node-ID passed in");
        return NULL;
    }

    for (i = 0; i < g_slist_length(hab->nodes); i ++) {
        bionet_node_t *node;

        node = g_slist_nth_data(hab->nodes, i);
        if (strcmp(node->id, node_id) == 0) return node;
    }

    return NULL;
}




int bionet_hab_get_num_nodes(const bionet_hab_t *hab) {
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_get_num_nodes(): NULL HAB passed in");
        return -1;
    }

    return g_slist_length(hab->nodes);
}


bionet_node_t *bionet_hab_get_node_by_index(bionet_hab_t *hab, unsigned int index) {
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_get_node_by_index(): NULL HAB passed in");
        return NULL;
    }

    if (index > bionet_hab_get_num_nodes(hab)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_hab_get_num_nodes(): Index is greater than number of nodes available.");
	return NULL;
    }

    return g_slist_nth_data(hab->nodes, index);
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
