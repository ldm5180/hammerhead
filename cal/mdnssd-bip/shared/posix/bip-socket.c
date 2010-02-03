
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "cal-mdnssd-bip.h"


#include <openssl/err.h>

int bip_socket_set_blocking(int s, int blocking) {
    // Make socket non-blocking
    int flags = fcntl(s, F_GETFL, 0);
    if (flags < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
            "%s: error getting scket flags: %s", 
                __FUNCTION__, strerror(errno));
        flags = 0;
        return -1;
    }
    if (fcntl(s, F_SETFL, flags | O_NONBLOCK) < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
            "%s: error setting scket flags: %s", 
                __FUNCTION__, strerror(errno));
        return -1;
    }

    return 0;

}

int bip_socket_connect(
        int s, 
        const struct sockaddr *address,
        socklen_t address_len,
        const char * peer_name,
        const bip_peer_network_info_t *net)
{
    int r;
    while((r = connect(s, address, address_len)) <0 && errno == EINTR);
    if (r < 0 && errno != EINPROGRESS ) {
        struct sockaddr_in *sin = (struct sockaddr_in *)address;
        g_log(
            CAL_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "%s: error connecting to peer '%s' at %s:%hu (%s): %s",
            __FUNCTION__,
            peer_name,
            net->hostname,
            net->port,
            inet_ntoa(sin->sin_addr),
            strerror(errno)
        );
        return -1;
    }
    return 0;
}

int bip_socket_tcp(void) {
    int s;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s: error making socket: %s", 
            __FUNCTION__, strerror(errno));
        return -1;
    }

    return s;
}



