
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cal.h"


cal_peer_t *cal_peer_new(const char *name) {
    cal_peer_t *peer;

    peer = (cal_peer_t *)calloc(1, sizeof(cal_peer_t));
    if (peer == NULL) {
        fprintf(stderr, "cal_peer_new(): out of memory\n");
        return NULL;
    }

    if (name != NULL) {
        peer->name = strdup(name);
        if (peer->name == NULL) {
            fprintf(stderr, "cal_peer_new(): out of memory\n");
            free(peer);
            return NULL;
        }
    }

    peer->addressing_scheme = CAL_AS_NONE;

    return peer;
}

