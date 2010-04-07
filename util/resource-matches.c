
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>

#include "internal.h"
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

    if (resource->node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_matches_habtype_habid_nodeid_resourceid(): resource with NULL node passed in!");
        return 0;
    }

    if (resource->node->hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_matches_habtype_habid_nodeid_resourceid(): resource with NULL hab passed in!");
        return 0;
    }

    if (!bionet_name_component_matches(resource->node->hab->type, hab_type)) return 0;
    if (!bionet_name_component_matches(resource->node->hab->id, hab_id)) return 0;
    if (!bionet_name_component_matches(resource->node->id, node_id)) return 0;
    if (!bionet_name_component_matches(resource->id, resource_id)) return 0;

    return 1;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
