
#include <stdio.h>
#include <stdlib.h>

#include "cal.h"


void cal_pd_free_peer(cal_peer_t *peer) {
    int i;


    if (peer == NULL) {
        return;
    }

    if (peer->name != NULL) {
        free(peer->name);
    }

    if (peer->unicast_address != NULL) {
        for (i = 0; i < peer->num_unicast_addresses; i ++) {
            if (peer->unicast_address[i] != NULL) {
                free(peer->unicast_address[i]);
            }
        }
        free(peer->unicast_address);
    }

    if (peer->user_data != NULL) {
        printf("user_data is not NULL!  Halp I'm leaking memory!\n");
    }

    free(peer);
}

