
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "bionet-util.h"


int bionet_stream_set_hab_type(bionet_stream_t *stream, const char *hab_type) {
    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_set_hab_type(): NULL Stream passed in!");
        errno = EINVAL;
        return -1;
    }

    if (stream->hab_type != NULL) free(stream->hab_type);

    if (hab_type == NULL) {
        stream->hab_type = NULL;
        return 0;
    }

    stream->hab_type = strdup(hab_type);
    if (stream->hab_type == NULL) {
        errno = ENOMEM;
        return -1;
    }

    return 0;
}


int bionet_stream_set_hab_id(bionet_stream_t *stream, const char *hab_id) {
    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_set_hab_id(): NULL Stream passed in!");
        errno = EINVAL;
        return -1;
    }

    if (stream->hab_id != NULL) free(stream->hab_id);

    if (hab_id == NULL) {
        stream->hab_id = NULL;
        return 0;
    }

    stream->hab_id = strdup(hab_id);
    if (stream->hab_id == NULL) {
        errno = ENOMEM;
        return -1;
    }

    return 0;
}


int bionet_stream_set_node_id(bionet_stream_t *stream, const char *node_id) {
    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_set_node_id(): NULL Stream passed in!");
        errno = EINVAL;
        return -1;
    }

    if (stream->node_id != NULL) free(stream->node_id);

    if (node_id == NULL) {
        stream->node_id = NULL;
        return 0;
    }

    stream->node_id = strdup(node_id);
    if (stream->node_id == NULL) {
        errno = ENOMEM;
        return -1;
    }

    return 0;
}

