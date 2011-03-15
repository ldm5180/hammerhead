
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>

#include <glib.h>

#include "bdm-client.h"
#include "libbdm-internal.h"

bionet_resource_t *bdm_cache_lookup_resource(const char *hab_type, const char *hab_id,
					     const char * node_id,
					     const char * resource_id) {
    
    bionet_node_t *node = NULL;
    bionet_resource_t *resource = NULL;

    /* Sanity check */
    if (hab_type == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_cache_lookup_resource(): NULL hab_type passed in!");
        return NULL;
    }

    if (hab_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_cache_lookup_resource(): NULL hab_id passed in!");
        return NULL;
    }

    if (node_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_cache_lookup_resource(): NULL node_id passed in!");
        return NULL;
    }

    if (resource_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_cache_lookup_resource(): NULL resource_id passed in!");
        return NULL;
    }

    /* find the node which would own this resource */
    node = bdm_cache_lookup_node(hab_type, hab_id, node_id);
    if (NULL == node) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "%s(): Unable to find node %s.%s.%s",
	      __FUNCTION__, hab_type, hab_id, node_id);
	return NULL;
    }

    resource = bionet_node_get_resource_by_id(node, resource_id);
    if (NULL == resource) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "%s(): Unable to find node %s.%s.%s:%s",
	      __FUNCTION__, hab_type, hab_id, node_id, resource_id);
    }

    return resource;

} /* bdm_cache_lookup_resource() */


bionet_resource_t *bdm_cache_lookup_resource_uid(const uint8_t node_uid[BDM_UUID_LEN], const char *resource_id) {
    bionet_node_t *node;

    node = bdm_cache_lookup_node_uid(node_uid);
    if (node == NULL) {
        return NULL;
    }

    return bionet_node_get_resource_by_id(node, resource_id);
}
