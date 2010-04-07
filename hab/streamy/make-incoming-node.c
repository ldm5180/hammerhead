
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <stdlib.h>

#include "hardware-abstractor.h"
#include "streamy-hab.h"


void make_incoming_node(bionet_hab_t *this_hab) {
    bionet_node_t *node;
    bionet_stream_t *stream;

    node = bionet_node_new(this_hab, "incoming");
    if (node == NULL) {
        fprintf(stderr, "error making 'incoming' Node\n");
        exit(1);
    }

    stream = bionet_stream_new(node, "incoming", BIONET_STREAM_DIRECTION_CONSUMER, "file");
    if (stream == NULL) {
        fprintf(stderr, "error making 'incoming' Stream\n");
        exit(1);
    }

    bionet_node_add_stream(node, stream);

    bionet_hab_add_node(this_hab, node);

    hab_report_new_node(node);
}

