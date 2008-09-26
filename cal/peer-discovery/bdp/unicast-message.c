
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>

#include "cal.h"
#include "bdp.h"


// send a message from the unicast socket to the peer's BDP unicast address
int bdp_unicast_message(void *buffer, int size, struct in_addr dest_addr, int dest_port) {
    int r;
    struct sockaddr_in dest_address;


    // 
    // make sure we have the unicast socket to send from
    //

    if (bdp_unicast_socket == -1) {
        bdp_unicast_socket = bdp_make_unicast_socket();
        if (bdp_unicast_socket < 0) {
            // an error message has already been logged
            return 0;
        }
    }


    //
    // send message
    //

    bzero(&dest_address, sizeof(struct sockaddr_in));
    dest_address.sin_family = AF_INET;
    dest_address.sin_port = htons(dest_port);
    dest_address.sin_addr = dest_addr;

    r = sendto(bdp_unicast_socket, buffer, size, 0, (struct sockaddr *)&dest_address, sizeof(struct sockaddr_in));
    if (r == -1) {
        printf("failed to unicast packet to peer: %s\n", strerror(errno));
        return 0;
    }
    if (r < size) {
        printf("short write while unicasting packet to peer: %s\n", strerror(errno));
        return 0;
    }


    return 1;
}

