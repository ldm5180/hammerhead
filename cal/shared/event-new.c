
#include <stdio.h>
#include <stdlib.h>

#include "cal.h"


cal_event_t *cal_event_new(cal_event_type_t type) {
    cal_event_t *event;

    event = (cal_event_t *)calloc(1, sizeof(cal_event_t));
    if (event == NULL) {
        fprintf(stderr, "cal_event_new(): out of memory\n");
        return NULL;
    }

    event->event_type = type;

    return event;
}

