
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>

#include "internal.h"
#include "bionet-util.h"


bionet_stream_direction_t bionet_stream_get_direction(const bionet_stream_t *stream) {
    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_get_direction(): NULL Stream passed in");
        return BIONET_STREAM_DIRECTION_INVALID;
    }

    return stream->direction;
}



const char *bionet_stream_direction_to_string(bionet_stream_direction_t direction) {
    switch (direction) {

        case BIONET_STREAM_DIRECTION_PRODUCER: return "Producer";

        case BIONET_STREAM_DIRECTION_CONSUMER: return "Consumer";

        default: return "(invalid)";

    }
}


bionet_stream_direction_t bionet_stream_direction_from_string(const char *direction_string) {

    if (strcasecmp(direction_string, "producer") == 0) {
        return BIONET_STREAM_DIRECTION_PRODUCER;

    } else if (strcasecmp(direction_string, "consumer") == 0) {
        return BIONET_STREAM_DIRECTION_CONSUMER;
    }

    return BIONET_STREAM_DIRECTION_INVALID;
}

