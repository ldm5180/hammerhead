
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "bionet-util.h"




int bionet_stream_set_host(
    bionet_stream_t *stream,
    const char *host
) {
    //
    // is the caller INSANE??!
    //

    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_set_host(): NULL Stream passed in!");
        return -1;
    }

    if (host == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_set_host(): NULL host-string passed in!");
        return -1;
    }


    if (stream->host != NULL) free(stream->host);
    stream->host = strdup(host);
    if (stream->host == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "out of memory!");
        return -1;
    }

    return 0;
}

