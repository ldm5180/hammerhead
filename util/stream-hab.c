
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <string.h>

#include "internal.h"
#include "bionet-util.h"


bionet_hab_t *bionet_stream_get_hab(const bionet_stream_t *stream) {
    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_hab(): NULL Stream passed in!");
        return NULL;
    }

    return stream->node->hab;
}



