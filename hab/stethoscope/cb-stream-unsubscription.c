
//
// Copyright (C) 2008-2009, Regents of the University of Colorado.
//


#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "hardware-abstractor.h"

extern int num_listeners;


void cb_stream_unsubscription(const char *client_id, const bionet_stream_t *stream) {
    num_listeners--;
}

