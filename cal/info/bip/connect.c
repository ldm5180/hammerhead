
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>

#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/types.h>

#include "cal.h"


int bip_connect_to_peer(cal_peer_t *peer) {
    int s;
    int r;

    struct addrinfo ai_hints;
    struct addrinfo *ai;


    if (peer->addressing_scheme != CAL_AS_IPv4) {
        fprintf(stderr, "bip: bip_connect_to_peer(): peer '%s' has unknown addressing scheme %d\n", peer->name, peer->addressing_scheme);
        return -1;
    }

    if (peer->as.ipv4.socket >= 0) return peer->as.ipv4.socket;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        fprintf(stderr, "bip: bip_connect_to_peer(): error making socket: %s\n", strerror(errno));
        return -1;
    }

    memset(&ai_hints, 0, sizeof(ai_hints));
    ai_hints.ai_family = AF_INET;  // IPv4
    ai_hints.ai_socktype = SOCK_STREAM;  // TCP
    r = getaddrinfo(peer->as.ipv4.hostname, NULL, &ai_hints, &ai);
    if (r != 0) {
        fprintf(stderr, "bip: bip_connect_to_peer(): error with getaddrinfo(\"%s\", ...): %s", peer->as.ipv4.hostname, gai_strerror(r));
        return -1;
    }
    if (ai == NULL) {
        fprintf(stderr, "bip: bip_connect_to_peer(): no results from getaddrinfo(\"%s\", ...)", peer->as.ipv4.hostname);
        return -1;
    }

    r = connect(s, ai->ai_addr, ai->ai_addrlen);
    if (r != 0) {
        struct sockaddr_in *sin = (struct sockaddr_in *)ai->ai_addr;
        fprintf(
            stderr,
            "bip: bip_connect_to_peer(): error connecting to peer '%s' at %s (%s): %s\n",
            peer->name,
            cal_peer_address_to_str(peer),
            inet_ntoa(sin->sin_addr),
            strerror(errno)
        );
        return -1;
    }

    peer->as.ipv4.socket = s;
    return s;
}

