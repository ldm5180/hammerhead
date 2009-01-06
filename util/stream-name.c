
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "bionet-util.h"


int bionet_stream_get_name(
    const bionet_stream_t *stream,
    char * name,
    int name_len
) {
    //
    // is the caller INSANE??!
    //

    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_name: NULL Stream passed in!");
        return -1;
    }

    if (stream->id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_name: passed-in Stream has NULL ID!");
        return -1;
    }

    if (stream->node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_name: passed-in Stream has NULL Node!");
        return -1;
    }

    if (stream->node->id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_name: passed-in Stream's Node has NULL ID!");
        return -1;
    }

    if (stream->node->hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_name: passed-in Stream's Node has NULL HAB!");
        return -1;
    }

    if (stream->node->hab->type == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_name: passed-in Stream's Node's HAB has NULL Type!");
        return -1;
    }

    if (stream->node->hab->id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_name: passed-in Stream's Node's HAB has NULL ID!");
        return -1;
    }

    if (name == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_name: passed-in name buffer is NULL!");
        return -1;
    }


    // 
    // looks ok
    //

    return snprintf(
        name,
        name_len,
        "%s.%s.%s:%s", 
	stream->node->hab->type,
        stream->node->hab->id,
        stream->node->id,
        stream->id
    );
}


