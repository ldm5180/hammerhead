
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <string.h>

#include "internal.h"
#include "bionet-util.h"


void bionet_stream_set_user_data(bionet_stream_t *stream, const void *user_data) {
    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_set_user_data(): NULL stream passed in!");
        return;
    }

    stream->user_data = user_data;
}


void *bionet_stream_get_user_data(const bionet_stream_t *stream) {
    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_user_data(): NULL stream passed in!");
        return NULL;
    }

    return (void *)stream->user_data;
}


