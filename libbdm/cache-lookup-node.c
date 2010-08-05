
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>

#include <glib.h>

#include "libbdm-internal.h"


bionet_node_t *bdm_cache_lookup_node(const char *hab_type, const char *hab_id, const char *node_id) {
    bionet_hab_t *hab;

    if (hab_type == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_cache_lookup_node(): NULL hab_type passed in!");
        return NULL;
    }

    if (hab_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_cache_lookup_node(): NULL hab_id passed in!");
        return NULL;
    }

    if (node_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_cache_lookup_node(): NULL node_id passed in!");
        return NULL;
    }

    hab = bdm_cache_lookup_hab(hab_type, hab_id);
    if (hab == NULL) {
        return NULL;
    }

    return bionet_hab_get_node_by_id(hab, node_id);
}

static int libbdm_find_node_by_guid(const void *vp_node, const void *vp_target) {
    const bionet_node_t *node = vp_node;
    const uint8_t *target_guid = vp_target;

    return memcmp(node->guid, target_guid, BDM_UUID_LEN);
}

bionet_node_t *bdm_cache_lookup_node_uid(const uint8_t node_uid[BDM_UUID_LEN]) {
    GSList *p;

    p = g_slist_find_custom(libbdm_nodes, (void*)node_uid, libbdm_find_node_by_guid);
    if (p == NULL) {
        return NULL;
    }

    return (bionet_node_t *)(p->data);
}
