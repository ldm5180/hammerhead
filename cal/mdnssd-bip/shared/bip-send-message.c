
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
int bip_send_message(const bip_peer_t *peer, uint8_t msg_type, const void *msg, uint32_t size) {
    int r;

    uint32_t msg_size;
    char msg_header[sizeof(msg_type) + sizeof(msg_size)];

    bip_peer_network_info_t *net = NULL;

    if (peer == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_send_message: NULL peer passed in");
        return -1;
    }

    net = bip_peer_get_connected_net(peer);
    if (net == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_send_message: no connection to peer '%s'", peer->peer_name);
        return -1;
    }


    msg_size = htonl(size);

    memcpy(msg_header, &msg_type, sizeof(msg_type));
    memcpy(msg_header + sizeof(msg_type), &msg_size, sizeof(msg_size));

    r = BIO_write(net->socket_bio, msg_header, sizeof(msg_header));
    if (r != sizeof(msg_header)) {
        if(BIO_should_retry(net->socket_bio)){
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_send_message: error sending header: timeout");
        } else {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_send_message: error sending header");
        }
        return -1;
    }

    if (size > 0) {
        const void * p = msg;
        while(size && (r = BIO_write(net->socket_bio, msg, size)) > 0) {
            p += r;
            size -= r;
        }
        if (r <= 0 ) {
            if(BIO_should_retry(net->socket_bio)){
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_send_message: error sending data: timeout");
            } else {
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_send_message: error sending data");
            }
            return -1;
        }
    }

    if (1 != BIO_flush(net->socket_bio)) {
        if(BIO_should_retry(net->socket_bio)){
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_send_message: error sending: timeout");
        } else {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_send_message: error sending");
        }
        return -1;
    }

    return 0;
}

