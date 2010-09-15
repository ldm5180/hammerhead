
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <stdlib.h>

#include <glib.h>

#include "cal-util.h"


cal_event_t *cal_event_new(cal_event_type_t type) {
    cal_event_t *event;

    event = (cal_event_t *)calloc(1, sizeof(cal_event_t));
    if (event == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "cal_event_new(): out of memory");
        return NULL;
    }

    event->type = type;

    return event;
}

