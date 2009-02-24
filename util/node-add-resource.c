
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "internal.h"
#include "bionet-util.h"


int bionet_node_add_resource(bionet_node_t *node, bionet_resource_t *resource) {
    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_add_resource(): NULL Node passed in");
        errno = EINVAL;
        return -1;
    }

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_add_resource(): NULL Resource passed in");
        errno = EINVAL;
        return -1;
    }

    // NOTE: in BIONET, Resource IDs dont have to be unique within a Node...
    node->resources = g_slist_append(node->resources, resource);

    resource->node = node;

    return 0;
}

