
#include <stdlib.h>

#include "cal-event.h"
#include "cal-peer.h"


void cal_event_free(cal_event_t *event) {
    if (event == NULL) {
        return;
    }

    cal_peer_free(event->peer);

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

