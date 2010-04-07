
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>

#include "internal.h"
#include "bionet-util.h"


bionet_node_t *bionet_stream_get_node(const bionet_stream_t *stream) {
    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_node(): NULL stream passed in!");
        return NULL;
    }

    return stream->node;
}



