
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "internal.h"
#include "bionet-util.h"


int bionet_node_add_stream(bionet_node_t *node, bionet_stream_t *stream) {
    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_add_stream(): NULL Node passed in");
        errno = EINVAL;
        return -1;
    }

    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_add_stream(): NULL Stream passed in");
        errno = EINVAL;
        return -1;
    }

    // NOTE: in BIONET, Resource IDs dont have to be unique within a Node...
    node->streams = g_slist_append(node->streams, stream);

    stream->node = node;

    return 0;
}

