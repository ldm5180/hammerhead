
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include "config.h"

#include "bip-socket-api.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

// This inclues the correct headers for the platform,
// and defiens the platform abstraction layer for mdns-sd
#include "bip-mdnssd-api.h"

#include "cal-mdnssd-bip.h"




int bip_read_from_peer(const char *peer_name, bip_peer_t *peer) {
    int r;
    int max_bytes_to_read;

    bip_peer_network_info_t *net;


    if (peer_name == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_read_from_peer: NULL peer_name passed in");
        return -1;
    }

    if (peer == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_read_from_peer: NULL peer passed in");
        return -1;
    }

    net = bip_peer_get_connected_net(peer);
    if (net == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_read_from_peer: peer '%s' is not connected", peer_name);
        return -1;
    }

    if (net->header_index < BIP_MSG_HEADER_SIZE) {
        max_bytes_to_read = BIP_MSG_HEADER_SIZE - net->header_index;
        r = BIO_read(net->socket_bio, &net->header[net->header_index], max_bytes_to_read);
        if (r < 0) {
            if( BIO_should_retry(net->socket_bio) ) return 0;

            //g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            //    "bip_read_from_peer(): error reading from peer %s: %s",
            //        peer_name, strerror(errno));
            return -1;
        } else if (r == 0) {
            // peer disconnects
            //g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            //    "bip_read_from_peer(): remote close %s", peer_name);
            return - 1;
        }

        net->header_index += r;

        if (net->header_index < BIP_MSG_HEADER_SIZE) return 0;

        // packet too big?
        net->msg_size = ntohl(*(uint32_t *)&net->header[BIP_MSG_HEADER_SIZE_OFFSET]);
        if (net->msg_size > BIP_MSG_MAX_SIZE) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_read_from_peer: incoming messages is %d bytes, max message size is %d bytes, dropping peer", net->msg_size, BIP_MSG_MAX_SIZE);
            return -1;
        }

        net->buffer = malloc(net->msg_size);
        if (net->buffer == NULL) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "bip_read_from_peer: out of memory!");
            return -1;
        }
        net->index = 0;
    }


    //
    // if we get here, the following things are true:
    //
    //     * we have the whole header
    //
    //     * msg_size is the expected size of the incoming packet payload
    //
    //     * buffer has been allocated to hold the whole payload
    //
    //     * the message index has the number of bytes currently in the
    //       message buffer (so it points to where the next byte should go)
    //


    max_bytes_to_read = net->msg_size - net->index;
    if (max_bytes_to_read > 0) {
        r = BIO_read(net->socket_bio, &net->buffer[net->index], max_bytes_to_read);
        if (r < 0) {
            if( BIO_should_retry(net->socket_bio) ) return 0;

            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "bip_read_from_peer(): error reading from peer %s: %s",
                    peer_name, strerror(errno));
            return -1;
        } else if (r == 0) {
            // peer disconnects in the middle of a message
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "bip_read_from_peer(): remote close %s", peer_name);
            return - 1;
        }

        net->index += r;

        if (r != max_bytes_to_read) return 0;
    }

    // yay a packet is in the peer's buffer!
    return 1;
}

