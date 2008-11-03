
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <string.h>

#include "bionet-util.h"


int bionet_resource_matches_id(const bionet_resource_t *resource, const char *id) {
    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_matches_id(): NULL resource passed in!");
        return 0;
    }

    if (id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_matches_id(): NULL id passed in!");
        return 0;
    }

    if ((strcmp(id, "*") == 0) || (strcmp(id, resource->id) == 0)) {
        return 1;
    } 

    return 0;
}


int bionet_resource_matches_habtype_habid_nodeid_resourceid(
    const bionet_resource_t *resource,
    const char *hab_type,
    const char *hab_id,
    const char *node_id,
    const char *resource_id
) {
    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_matches_habtype_habid_nodeid_resourceid(): NULL resource passed in!");
        return 0;
    }

    if (resource->id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_matches_habtype_habid_nodeid_resourceid(): resource with NULL id passed in!");
        return 0;
    }

    if (resource->node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_matches_habtype_habid_nodeid_resourceid(): resource with NULL node passed in!");
        return 0;
    }

    if (resource->node->id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_matches_habtype_habid_nodeid_resourceid(): resource with NULL node_id passed in!");
        return 0;
    }

    if (resource->node->hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_matches_habtype_habid_nodeid_resourceid(): resource with NULL hab passed in!");
        return 0;
    }

    if (resource->node->hab->type == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_matches_habtype_habid_nodeid_resourceid(): resource with NULL hab_type passed in!");
        return 0;
    }

    if (resource->node->hab->id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_matches_habtype_habid_nodeid_resourceid(): resource with NULL hab_id passed in!");
        return 0;
    }

    if (hab_type == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_matches_habtype_habid_nodeid_resourceid(): NULL hab_type passed in!");
        return 0;
    }

    if (hab_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_matches_habtype_habid_nodeid_resourceid(): NULL hab_id passed in!");
        return 0;
    }

    if (node_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_matches_habtype_habid_nodeid_resourceid(): NULL node_id passed in!");
        return 0;
    }

    if (resource_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_matches_habtype_habid_nodeid_resourceid(): NULL resource_id passed in!");
        return 0;
    }

    if (
        (
            (strcmp(hab_type, "*") == 0) ||
            (strcmp(hab_type, resource->node->hab->type) == 0) 
        ) &&
        (
            (strcmp(hab_id, "*") == 0) ||
            (strcmp(hab_id, resource->node->hab->id) == 0) 
        ) &&
        (
            (strcmp(node_id, "*") == 0) ||
            (strcmp(node_id, resource->node->id) == 0)
        ) &&
        (
            (strcmp(resource_id, "*") == 0) ||
            (strcmp(resource_id, resource->id) == 0)
        )
    ) {
        return 1;
    } 

    return 0;
}

