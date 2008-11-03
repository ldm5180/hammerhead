
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <string.h>

#include "bionet-util.h"


int bionet_node_matches_id(const bionet_node_t *node, const char *id) {
    if ((strcmp(id, "*") == 0) || (strcmp(id, node->id) == 0)) {
        return 1;
    } 

    return 0;
}


int bionet_node_matches_habtype_habid_nodeid(const bionet_node_t *node, const char *hab_type, const char *hab_id, const char *node_id) {
    if (
        (
            (strcmp(hab_type, "*") == 0) ||
            (strcmp(hab_type, node->hab->type) == 0) 
        ) &&
        (
            (strcmp(hab_id, "*") == 0) ||
            (strcmp(hab_id, node->hab->id) == 0) 
        ) &&
        (
            (strcmp(node_id, "*") == 0) ||
            (strcmp(node_id, node->id) == 0)
        )
    ) {
        return 1;
    } 

    return 0;
}

