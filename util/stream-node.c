
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <string.h>

#ifdef BIONET_21_API
#include "internal.h"
#include "bionet-util-2.1.h"


bionet_node_t *bionet_stream_get_node(const bionet_stream_t *stream) {
    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_node(): NULL stream passed in!");
        return NULL;
    }

    return stream->node;
}

#endif
