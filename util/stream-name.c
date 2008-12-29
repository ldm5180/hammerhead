
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef BIONET_21_API
#include "internal.h"
#include "bionet-util-2.1.h"
#else
#include "bionet-util.h"
#endif



const char *bionet_stream_get_name(const bionet_stream_t *stream) {
    static char stream_name[(4*BIONET_NAME_COMPONENT_MAX_LEN) + 4];
    int r;


    //
    // is the caller INSANE??!
    //

    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_name: NULL Stream passed in!");
        return NULL;
    }

    if (stream->id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_name: passed-in Stream has NULL ID!");
        return NULL;
    }

    if (stream->node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_name: passed-in Stream has NULL Node!");
        return NULL;
    }

    if (stream->node->id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_name: passed-in Stream's Node has NULL ID!");
        return NULL;
    }

    if (stream->node->hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_name: passed-in Stream's Node has NULL HAB!");
        return NULL;
    }

    if (stream->node->hab->type == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_name: passed-in Stream's Node's HAB has NULL Type!");
        return NULL;
    }

    if (stream->node->hab->id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_name: passed-in Stream's Node's HAB has NULL ID!");
        return NULL;
    }


    // 
    // look ok
    //

    r = snprintf(
        stream_name,
        sizeof(stream_name),
        "%s.%s.%s:%s", 
        stream->node->hab->type,
        stream->node->hab->id,
        stream->node->id,
        stream->id
    );
    if ((r >= sizeof(stream_name)) || (r < 0)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_name: error stringifying stream name");
        return NULL;
    }

    return stream_name;
}

