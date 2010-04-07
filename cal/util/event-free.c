
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>

#include "cal-event.h"


void cal_event_free(cal_event_t *event) {
    if (event == NULL) {
        return;
    }

    if (event->peer_name != NULL) {
        free(event->peer_name);
    }

    if (event->msg.buffer != NULL) {
        free(event->msg.buffer);
    }

    if (event->topic != NULL) {
        free(event->topic);
    }

    free(event);
}

