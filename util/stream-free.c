
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdlib.h>

#include "internal.h"
#include "bionet-util.h"


void bionet_stream_free(bionet_stream_t *stream) {
    if (stream == NULL) {
        return;
    }

    if (stream->user_data != NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_free() called with stream %s, user_data is not NULL, leaking memory now", stream->id);
    }

    if (stream->id != NULL) {
        free(stream->id);
    }

    if (stream->name != NULL) {
        free(stream->name);
    }

    if (stream->type != NULL) {
        free(stream->type);
    }

    free(stream);
}

