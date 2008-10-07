
#include <stdio.h>
#include <stdlib.h>

#include "cal-peer.h"


void cal_peer_free(cal_peer_t *peer) {
    if (peer == NULL) {
        return;
    }

    if (peer->name != NULL) {
        free(peer->name);
    }


    switch (peer->addressing_scheme) {
        case CAL_AS_NONE: {
            break;
        }

        case CAL_AS_IPv4: {
            if (peer->as.ipv4.hostname != NULL) {
                free(peer->as.ipv4.hostname);
            }
            break;
        }

        default: {
            fprintf(stderr, "cal_peer_free(): peer has unknown addressing scheme %d!\n", peer->addressing_scheme);
            break;
        }
    }


    if (peer->user_data != NULL) {
        printf("user_data is not NULL!  Halp I'm leaking memory!\n");
    }

    free(peer);
}

