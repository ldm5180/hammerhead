
#include <stdlib.h>

#include "cal.h"


void cal_pd_free_event(cal_event_t *event) {
    int i;


    if (event == NULL) {
        return;
    }

    if (event->peer.name != NULL) {
        free(event->peer.name);
    }

    for (i = 0; i < event->peer.num_unicast_addresses; i ++) {
        free(event->peer.unicast_address[i]);
    }

    free(event);
}

