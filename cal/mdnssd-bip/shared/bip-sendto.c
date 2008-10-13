
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




int bip_sendto(const cal_peer_t *peer, const void *msg, int size) {
    int r;

    uint8_t msg_type;
    uint32_t msg_size;

    printf("bip_sendto: sending \"%s\" (%d bytes) to %s\n", (char *)msg, size, peer->name);

    msg_type = BIP_MSG_TYPE_MESSAGE;
    msg_size = htonl(size);

    // FIXME: this should be one write

    r = write(peer->as.ipv4.socket, &msg_type, sizeof(msg_type));
    if (r != sizeof(msg_type)) {
        return -1;
    }

    r = write(peer->as.ipv4.socket, &msg_size, sizeof(msg_size));
    if (r != sizeof(msg_size)) {
        return -1;
    }

    return write(peer->as.ipv4.socket, msg, size);
}

