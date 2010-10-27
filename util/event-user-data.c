
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>

#include "internal.h"
#include "bionet-util.h"


void bionet_event_set_user_data(bionet_event_t *event, const void *user_data) {
    if (event == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_event_set_user_data(): NULL event passed in!");
        return;
    }

    event->user_data = user_data;
}


void *bionet_event_get_user_data(const bionet_event_t *event) {
    if (event == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_event_get_user_data(): NULL event passed in!");
        return NULL;
    }

    return (void *)event->user_data;
}


