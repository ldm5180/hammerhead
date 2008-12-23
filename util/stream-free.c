
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdlib.h>

#ifdef BIONET_21_API
#include "internal.h"
#include "bionet-util-2.1.h"
#else
#include "bionet-util.h"
#endif


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

    if (stream->type != NULL) {
        free(stream->type);
    }

    free(stream);
}

