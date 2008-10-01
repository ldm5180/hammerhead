
#include <stdlib.h>

#include "cal.h"


void cal_event_free(cal_event_t *event) {
    if (event == NULL) {
        return;
    }

    cal_peer_free(event->peer);

    free(event);
}

