
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




int bip_read_from_peer(const char *peer_name, bip_peer_t *peer) {
    int r;
    int max_bytes_to_read;


    if (peer->net->header_index < BIP_MSG_HEADER_SIZE) {
        max_bytes_to_read = BIP_MSG_HEADER_SIZE - peer->net->header_index;
        r = read(peer->net->socket, &peer->net->header[peer->net->header_index], max_bytes_to_read);
        if (r < 0) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_read_from_peer(): error reading from peer %s: %s", peer_name, strerror(errno));
            return -1;
        } else if (r == 0) {
            // peer disconnects
            return - 1;
        }

        peer->net->header_index += r;

        if (peer->net->header_index < BIP_MSG_HEADER_SIZE) return 0;

        // packet too big?
        peer->net->msg_size = ntohl(*(uint32_t *)&peer->net->header[BIP_MSG_HEADER_SIZE_OFFSET]);
        if (peer->net->msg_size > BIP_MSG_MAX_SIZE) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_read_from_peer: incoming messages is %d bytes, max message size is %d bytes, dropping client", peer->net->msg_size, BIP_MSG_MAX_SIZE);
            return -1;
        }

        peer->net->buffer = malloc(peer->net->msg_size);
        if (peer->net->buffer == NULL) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "bip_read_from_peer: out of memory!");
            return -1;
        }
        peer->net->index = 0;
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


    max_bytes_to_read = peer->net->msg_size - peer->net->index;
    if (max_bytes_to_read > 0) {
        r = read(peer->net->socket, &peer->net->buffer[peer->net->index], max_bytes_to_read);
        if (r < 0) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_read_from_peer(): error reading from peer %s: %s", peer_name, strerror(errno));
            return -1;
        } else if (r == 0) {
            // peer disconnects
            return - 1;
        }

        peer->net->index += r;

        if (r != max_bytes_to_read) return 0;
    }

    // yay a packet is in the peer's buffer!
    return 1;
}

