
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#ifdef BIONET_21_API
#include "internal.h"
#include "bionet-util-2.1.h"
#else
#include "bionet-util.h"
#endif

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

