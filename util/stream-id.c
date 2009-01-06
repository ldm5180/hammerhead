
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <string.h>

#include "internal.h"
#include "bionet-util.h"


const char *bionet_stream_get_id(const bionet_stream_t *stream) {
    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_id(): NULL stream passed in!");
        return "(invalid)";
    }

    return stream->id;
}

