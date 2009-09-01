
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>

#include <glib.h>

#include "libbdm-internal.h"


bionet_resource_t *bdm_cache_lookup_resource(const char *hab_type, const char *hab_id, const char *node_id, const char *resource_id) {
    bionet_node_t *node;

    node = bdm_cache_lookup_node(hab_type, hab_id, node_id);
    if (node == NULL) {
        return NULL;
    }

    return bionet_node_get_resource_by_id(node, resource_id);
}


