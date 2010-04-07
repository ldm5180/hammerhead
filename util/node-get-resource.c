
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>

#include <glib.h>

#include "internal.h"
#include "bionet-util.h"


int bionet_node_get_num_resources(const bionet_node_t *node) {
    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_get_num_resources(): NULL Node passed in");
        errno = EINVAL;
        return -1;
    }
    return g_slist_length(node->resources);
}


bionet_resource_t *bionet_node_get_resource_by_index(const bionet_node_t *node, unsigned int index) {
    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_get_resource_by_index(): NULL Node passed in");
        errno = EINVAL;
        return NULL;
    }
    return g_slist_nth_data(node->resources, index);
}


bionet_resource_t *bionet_node_get_resource_by_id(const bionet_node_t *node, const char *resource_id) {
    GSList *i;

    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_get_resource_by_id(): NULL Node passed in");
        errno = EINVAL;
        return NULL;
    }

    if (resource_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_get_resource_by_id(): NULL Resource-ID passed in");
        errno = EINVAL;
        return NULL;
    }

    for (i = node->resources; i != NULL; i = i->next) {
        bionet_resource_t *resource = i->data;
        if (strcmp(resource->id, resource_id) == 0) return resource;
    }

    errno = ENOENT;
    return NULL;
}

