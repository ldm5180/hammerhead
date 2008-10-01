
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


int bip_init_publisher(cal_peer_t *this, void (*callback)(cal_event_t *event)) {
    bip_socket = bip_make_listening_socket();
    if (bip_socket < 0) {
        return -1;
    }


    {
        socklen_t len;
        int r;

        len = sizeof(this->addr);
        r = getsockname(bip_socket, &this->addr, &len);
        if (r < 0) {
            printf("error in getsockname: %s\n", strerror(errno));
            return -1;
        }
    }


    this->socket = bip_socket;

    return bip_socket;
}


void bip_publish(char *topic, void *msg, int size) {
    printf("bip_publish is not implemented yet\n");
}


int bip_publisher_read(void) {
    int r;
    int i;
    char buffer[100];

    r = read(bip_socket, buffer, sizeof(buffer));
    if (r < 0) {
        printf("error in bip_read(): %s\n", strerror(errno));
        return -1;
    }

    printf("bip_read got %d bytes:\n", r);
    for (i = 0; i < r; i ++) {
        printf("   %c\n", isprint(buffer[i]) ? buffer[i] : '.');
    }
    return r;
}

