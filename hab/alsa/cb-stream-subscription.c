
//
// Copyright (C) 2008-2009, Regents of the University of Colorado.
//


#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <sys/poll.h>
#include <glib.h>

#include "hardware-abstractor.h"
#include "alsa-hab.h"


void cb_stream_subscription(const char *client_id, const bionet_stream_t *stream) {
    printf("client '%s' subscribes to %s:%s\n", client_id, bionet_node_get_id(bionet_stream_get_node(stream)), bionet_stream_get_id(stream));
}

