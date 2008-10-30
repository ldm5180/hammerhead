
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <string.h>

#include <glib.h>

#include "bionet.h"


bionet_stream_t *bionet_cache_lookup_stream(const char *hab_type, const char *hab_id, const char *node_id, const char *stream_id) {
    bionet_node_t *node;

    node = bionet_cache_lookup_node(hab_type, hab_id, node_id);
    if (node == NULL) {
        return NULL;
    }

    return bionet_node_get_stream_by_id(node, stream_id);
}


