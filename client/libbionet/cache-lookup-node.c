
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>

#include <glib.h>

#include "bionet.h"


bionet_node_t *bionet_cache_lookup_node(const char *hab_type, const char *hab_id, const char *node_id) {
    bionet_hab_t *hab;

    if (hab_type == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_cache_lookup_node(): NULL hab_type passed in!");
        return NULL;
    }

    if (hab_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_cache_lookup_node(): NULL hab_id passed in!");
        return NULL;
    }

    if (node_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_cache_lookup_node(): NULL node_id passed in!");
        return NULL;
    }

    hab = bionet_cache_lookup_hab(hab_type, hab_id);
    if (hab == NULL) {
        return NULL;
    }

    return bionet_hab_get_node_by_id(hab, node_id);
}


