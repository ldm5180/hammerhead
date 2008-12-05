
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/types.h>

#include "cal-mdnssd-bip.h"




int bip_net_connect(const char *peer_name, bip_peer_network_info_t *net) {
    int s;
    int r;

    struct addrinfo ai_hints;
    struct addrinfo *ai;


    if (net == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_net_connect: NULL net passed in");
        return -1;
    }


    if (net->socket != -1) return net->socket;


    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_net_connect: error making socket: %s", strerror(errno));
        return -1;
    }

    memset(&ai_hints, 0, sizeof(ai_hints));
    ai_hints.ai_family = AF_INET;  // IPv4
    ai_hints.ai_socktype = SOCK_STREAM;  // TCP
    r = getaddrinfo(net->hostname, NULL, &ai_hints, &ai);
    if (r != 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_net_connect: error with getaddrinfo(\"%s\", ...): %s", net->hostname, gai_strerror(r));
        return -1;
    }
    if (ai == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_net_connect: no results from getaddrinfo(\"%s\", ...)", net->hostname);
        return -1;
    }

    ((struct sockaddr_in *)ai->ai_addr)->sin_port = htons(net->port);

    r = connect(s, ai->ai_addr, ai->ai_addrlen);
    if (r != 0) {
#if 0
        struct sockaddr_in *sin = (struct sockaddr_in *)ai->ai_addr;
        g_log(
            CAL_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "bip_net_connect: error connecting to peer '%s' at %s:%hu (%s): %s",
            peer_name,
            net->hostname,
            net->port,
            inet_ntoa(sin->sin_addr),
            strerror(errno)
        );
#endif

        return -1;
    }

    net->socket = s;

    return s;
}

