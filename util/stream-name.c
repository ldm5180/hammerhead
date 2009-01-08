
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "bionet-util.h"


const char *bionet_stream_get_name(bionet_stream_t *stream) {
    char buf[4 * BIONET_NAME_COMPONENT_MAX_LEN];
    int r;

    //
    // is the caller INSANE??!
    //

    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_name: NULL Stream passed in!");
        errno = EINVAL;
        return NULL;
    }

    if (stream->id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_name: passed-in Stream has NULL ID!");
        errno = EINVAL;
        return NULL;
    }

    if (stream->node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_name: passed-in Stream has NULL Node!");
        errno = EINVAL;
        return NULL;
    }

    if (stream->node->id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_name: passed-in Stream's Node has NULL ID!");
        errno = EINVAL;
        return NULL;
    }

    if (stream->node->hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_name: passed-in Stream's Node has NULL HAB!");
        errno = EINVAL;
        return NULL;
    }

    if (stream->node->hab->type == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_name: passed-in Stream's Node's HAB has NULL Type!");
        errno = EINVAL;
        return NULL;
    }

    if (stream->node->hab->id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_name: passed-in Stream's Node's HAB has NULL ID!");
        errno = EINVAL;
        return NULL;
    }


    // 
    // looks ok
    //

    if (stream->name != NULL) return stream->name;

    r = snprintf(
        buf,
        sizeof(buf),
        "%s.%s.%s:%s",
        stream->node->hab->type,
        stream->node->hab->id,
        stream->node->id,
        stream->id
    );

    if (r >= sizeof(buf)) {
        errno = EINVAL;
        g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "bionet_stream_get_name(): Stream name %s.%s.%s:%s too long!",
            stream->node->hab->type,
            stream->node->hab->id,
            stream->node->id,
            stream->id
        );
        return NULL;
    }

    stream->name = strdup(buf);
    if (stream->name == NULL) {
        g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "bionet_stream_get_name(): out of memory!"
        );
        return NULL;
    }

    return stream->name;
}


const char *bionet_stream_get_local_name(bionet_stream_t *stream) {
    char buf[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    int r;

    //
    // is the caller INSANE??!
    //

    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_local_name: NULL Stream passed in!");
        errno = EINVAL;
        return NULL;
    }

    if (stream->id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_local_name: passed-in Stream has NULL ID!");
        errno = EINVAL;
        return NULL;
    }

    if (stream->node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_local_name: passed-in Stream has NULL Node!");
        errno = EINVAL;
        return NULL;
    }

    if (stream->node->id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_local_name: passed-in Stream's Node has NULL ID!");
        errno = EINVAL;
        return NULL;
    }


    // 
    // looks ok
    //

    if (stream->local_name != NULL) return stream->local_name;

    r = snprintf(
        buf,
        sizeof(buf),
        "%s:%s",
        stream->node->id,
        stream->id
    );

    if (r >= sizeof(buf)) {
        errno = EINVAL;
        g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "bionet_stream_get_local_name(): local Stream name %s:%s too long!",
            stream->node->id,
            stream->id
        );
        return NULL;
    }

    stream->local_name = strdup(buf);
    if (stream->local_name == NULL) {
        g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "bionet_stream_get_local_name(): out of memory!"
        );
        return NULL;
    }

    return stream->local_name;
}

