
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "hardware-abstractor.h"
#include "streamy-hab.h"


void cb_stream_unsubscription(const char *client_id, const bionet_stream_t *stream) {
    printf("client '%s' unsubscribes from %s\n", client_id, bionet_stream_get_local_name(stream));
}

