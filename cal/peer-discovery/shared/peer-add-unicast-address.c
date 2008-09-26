
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cal.h"


int cal_peer_add_unicast_address(cal_peer_t *peer, const char *new_addr) {
    char **tmp;

    peer->num_unicast_addresses ++;
    tmp = (char **)realloc(peer->unicast_address, (peer->num_unicast_addresses * sizeof(char*)));
    if (tmp == NULL) {
        printf("out of memory!\n");
        peer->num_unicast_addresses --;
        return -1;
    }
    peer->unicast_address = tmp;

    peer->unicast_address[peer->num_unicast_addresses - 1] = strdup(new_addr);
    if (peer->unicast_address[peer->num_unicast_addresses - 1] == NULL) {
        printf("out of memory!\n");
        peer->num_unicast_addresses --;
        return -1;
    }

    return 0;
}

