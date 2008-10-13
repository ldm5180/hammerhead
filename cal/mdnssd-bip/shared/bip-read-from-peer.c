
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




int bip_read_from_peer(cal_peer_t *peer) {
    int r;
    int max_bytes_to_read;
    int payload_size;


    if (peer->index < BIP_MSG_HEADER_SIZE) {
        max_bytes_to_read = BIP_MSG_HEADER_SIZE - peer->index;
        r = read(peer->as.ipv4.socket, &peer->buffer[peer->index], max_bytes_to_read);
        if (r < 0) {
            fprintf(stderr, "bip_read_from_peer(): error reading from peer %s: %s\n", peer->name, strerror(errno));
            return -1;
        } else if (r == 0) {
            fprintf(stderr, "bip_read_from_peer(): peer %s disconnects\n", peer->name);
            return - 1;
        }

        peer->index += r;

        if (peer->index < BIP_MSG_HEADER_SIZE) return 0;
    }

    payload_size = ntohl(*(uint32_t *)&peer->buffer[BIP_MSG_HEADER_SIZE_OFFSET]);

    max_bytes_to_read = (payload_size + BIP_MSG_HEADER_SIZE) - peer->index;
    if (max_bytes_to_read > 0) {
        r = read(peer->as.ipv4.socket, &peer->buffer[peer->index], max_bytes_to_read);
        if (r < 0) {
            fprintf(stderr, "bip_read_from_peer(): error reading from peer %s: %s\n", peer->name, strerror(errno));
            return -1;
        } else if (r == 0) {
            fprintf(stderr, "bip_read_from_peer(): peer %s disconnects\n", peer->name);
            return - 1;
        }

        peer->index += r;

        if (r != max_bytes_to_read) return 0;
    }

    // yay a packet is in the peer's buffer!
    return 1;
}

