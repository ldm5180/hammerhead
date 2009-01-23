
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


void cb_stream_data(const char *client_id, bionet_stream_t *stream, const void *data, unsigned int size) {
    printf("client '%s' sends %d bytes to stream %s\n", client_id, size, bionet_stream_get_local_name(stream));
}

