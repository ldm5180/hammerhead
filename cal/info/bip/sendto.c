
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <sys/socket.h>

#include "cal.h"
#include "bip.h"


int bip_sendto(cal_peer_t *peer, void *msg, int size) {
    printf("should send \"%s\" (%d bytes) to %s\n", (char *)msg, size, peer->name);
    return -1;
}

