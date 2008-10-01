
#include <stdio.h>
#include <stdlib.h>

#include "cal.h"


void cal_peer_free(cal_peer_t *peer) {
    if (peer == NULL) {
        return;
    }

    if (peer->name != NULL) {
        free(peer->name);
    }

    if (peer->user_data != NULL) {
        printf("user_data is not NULL!  Halp I'm leaking memory!\n");
    }

    free(peer);
}

