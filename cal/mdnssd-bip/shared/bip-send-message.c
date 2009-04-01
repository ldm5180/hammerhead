
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


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

#include <glib.h>
#include <dns_sd.h>

#include "cal-mdnssd-bip.h"




//
// We're currently not limiting the size of BIP messages.  This sometimes
// worries Coverity.
//
// Maybe in the future we'll limit max message sizes in some configurable
// way, as part of a security mechanism to try to resist DOS attacks.
//
// FIXME: Also see Issue #343 at <http://bioserve.colorado.edu/issue-tracker/?module=issues&action=view&issueid=343>
//
// coverity[ -tainted_data_sink : arg-4 ]
int bip_send_message(const char *peer_name, const bip_peer_t *peer, uint8_t msg_type, const void *msg, uint32_t size) {
    int r;

    uint32_t msg_size;

    bip_peer_network_info_t *net = NULL;


    if (peer_name == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_send_message: NULL peer_name passed in");
        return -1;
    }

    if (peer == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_send_message: NULL peer passed in");
        return -1;
    }

    net = bip_peer_get_connected_net(peer);
    if (net == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_send_message: no connection to peer '%s'", peer_name);
        return -1;
    }


    msg_size = htonl(size);

    // FIXME: this should be one write

    r = write(net->socket, &msg_type, sizeof(msg_type));
    if (r != sizeof(msg_type)) {
        return -1;
    }

    r = write(net->socket, &msg_size, sizeof(msg_size));
    if (r != sizeof(msg_size)) {
        return -1;
    }

    if (size == 0) return 0;

    r = write(net->socket, msg, size);
    if (r != size) {
        return -1;
    }

    return 0;
}

