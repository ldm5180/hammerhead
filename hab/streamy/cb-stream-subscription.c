
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware-abstractor.h"
#include "streamy-hab.h"


void cb_stream_subscription(const char *client_id, const bionet_stream_t *stream) {
    printf("queueing publishing on stream %s because client '%s' subscribed\n", bionet_stream_get_local_name(stream), client_id);
    g_idle_add(publish_file, (void*)stream);
}

