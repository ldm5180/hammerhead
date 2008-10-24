
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




int bip_send_message(const char *peer_name, const bip_peer_t *peer, uint8_t msg_type, const void *msg, uint32_t size) {
    int r;

    uint32_t msg_size;

    if (peer == NULL) return -1;
    if (peer->net == NULL) return -1;
    if (peer->net->socket == -1) return -1;

    // printf("bip_send_message: sending message type %d \"%s\" (%d bytes) to %s\n", msg_type, (char *)msg, size, peer_name);

    msg_type = msg_type;
    msg_size = htonl(size);

    // FIXME: this should be one write

    r = write(peer->net->socket, &msg_type, sizeof(msg_type));
    if (r != sizeof(msg_type)) {
        return -1;
    }

    r = write(peer->net->socket, &msg_size, sizeof(msg_size));
    if (r != sizeof(msg_size)) {
        return -1;
    }

    if (size == 0) return 0;

    r = write(peer->net->socket, msg, size);
    if (r != sizeof(msg_size)) {
        return -1;
    }

    return 0;
}

