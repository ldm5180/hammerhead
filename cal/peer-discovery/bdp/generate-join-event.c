
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cal.h"
#include "bdp.h"


void bdp_generate_join_event(cal_peer_t *peer) {
    cal_event_t *event;
    int r;

    event = (cal_event_t *)calloc(1, sizeof(cal_event_t));
    if (event == NULL) {
        printf("bdp_generate_join_event: error allocating memory for event: %s\n", strerror(errno));
        return;
    }


    //
    // make the event and pass it up to the user
    //

    event->event_type = CAL_EVENT_JOIN;
    event->peer.name = strdup(peer->name);
    event->peer.num_unicast_addresses = 0;

    r = write(bdp_pipe_fds[1], &event, sizeof(event));  // heh
    if (r < 0) {
        printf("bdp_generate_join_event: error writing event: %s\n", strerror(errno));
    } else if (r != sizeof(event)) {
        printf("bdp_generate_join_event: short write while writing event\n");
    }
}

