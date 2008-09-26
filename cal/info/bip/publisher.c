
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
    char tmp_str[100];
    struct sockaddr_in my_address;

    bip_socket = bip_make_listening_socket();
    if (bip_socket < 0) {
        return -1;
    }


    {
        socklen_t len;
        int r;

        len = sizeof(my_address);
        r = getsockname(bip_socket, (struct sockaddr *)&my_address, &len);
        if (r < 0) {
            printf("error in getsockname: %s\n", strerror(errno));
            return -1;
        }
    }

    snprintf(tmp_str, sizeof(tmp_str), "bip://this-host:%d", ntohs(my_address.sin_port));

    this->num_unicast_addresses = 1;
    this->unicast_address = (char **)calloc(1, sizeof(char *));
    this->unicast_address[0] = strdup(tmp_str);

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

