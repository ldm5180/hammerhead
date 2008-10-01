
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/types.h>

#include "cal.h"


int bip_connect_to_peer(cal_peer_t *peer) {
    int s;
    int r;
    struct sockaddr_in *sin = (struct sockaddr_in *)&peer->addr;

    if (peer->socket >= 0) return peer->socket;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        fprintf(stderr, "bip: bip_connect_to_peer(): error making socket: %s\n", strerror(errno));
        return -1;
    }

    r = connect(s, &peer->addr, sizeof(peer->addr));
    if (r != 0) {
        fprintf(stderr, "bip: bip_connect_to_peer(): error connecting to %s:%hu: %s\n", inet_ntoa(sin->sin_addr), ntohs(sin->sin_port), strerror(errno));
        return -1;
    }

    peer->socket = s;
    return s;
}

