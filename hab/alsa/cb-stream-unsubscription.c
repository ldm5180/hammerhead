
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


void cb_stream_unsubscription(const char *client_id, const bionet_stream_t *stream) {
    printf("client '%s' unsubscribes from %s\n", client_id, bionet_stream_get_local_name(stream));
    disconnect_client((bionet_stream_t *)stream, NULL);
}

