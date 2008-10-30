
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <string.h>

#include "bionet-util.h"


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

