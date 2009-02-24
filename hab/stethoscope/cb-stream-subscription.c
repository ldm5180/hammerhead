
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware-abstractor.h"

extern int num_listeners;
 

void cb_stream_subscription(const char *client_id, const bionet_stream_t *stream) {
    fprintf(stderr, "subscriber identified: %s\n", client_id);
    num_listeners++;
}

