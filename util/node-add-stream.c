
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


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

    if(bionet_node_get_stream_by_id(node, stream->id)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
               "bionet_node_add_stream(): Node %s already has a stream with id %s",
               node->id, stream->id);
        errno = EINVAL;
        return -1;

    }

    node->streams = g_slist_append(node->streams, stream);

    stream->node = node;

    return 0;
}

