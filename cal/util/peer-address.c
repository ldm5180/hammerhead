
#include <stdio.h>

#include "cal.h"


void cal_peer_set_addressing_scheme(cal_peer_t *peer, cal_as_type_t addressing_scheme) {
    switch (addressing_scheme) {
        case CAL_AS_NONE: {
            peer->addressing_scheme = addressing_scheme;
            return;
        }

        case CAL_AS_IPv4: {
            peer->addressing_scheme = addressing_scheme;
            peer->as.ipv4.socket = -1;
            peer->as.ipv4.hostname = NULL;
            peer->as.ipv4.port = 0;
            return;
        }

        default: {
            fprintf(stderr, "cal_peer_set_addressing_scheme(): unknown addressing scheme %d\n", addressing_scheme);
            return;
        }
    }
}


const char *cal_peer_address_to_str(const cal_peer_t *peer) {
    static char str[100];
    int r;

    switch (peer->addressing_scheme) {
        case CAL_AS_NONE: {
            r = snprintf(str, sizeof(str), "(no address)");
            if (r >= sizeof(str)) {
                fprintf(stderr, "cal_peer_address_to_str(): address too big to fit in string!\n");
                return "invalid";
            }
            return str;
        }

        case CAL_AS_IPv4: {
            r = snprintf(str, sizeof(str), "IPv4://%s:%hu", peer->as.ipv4.hostname, peer->as.ipv4.port);
            if (r >= sizeof(str)) {
                fprintf(stderr, "cal_peer_address_to_str(): address too big to fit in string!\n");
                return "invalid";
            }
            return str;
        }

        default: {
            fprintf(stderr, "cal_peer_address_to_str(): peer has unknown addressing scheme %d\n", peer->addressing_scheme);
            return "invalid";
        }
    }

    // NOT REACHED
}

