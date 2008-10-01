
#include <stdlib.h>

#include "cal.h"


void cal_pd_free_event(cal_event_t *event) {
    if (event == NULL) {
        return;
    }

    if (event->peer.name != NULL) {
        free(event->peer.name);
    }

    free(event);
}

