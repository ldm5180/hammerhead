
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>

#include "cal.h"
#include "bdp.h"


// send a message from the unicast socket to the multicast group
int bdp_mcast_message(void *buffer, int size) {
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
    dest_address.sin_port = htons(BDP_MCAST_PORT);
    if (!inet_aton(BDP_MCAST_ADDR, (struct in_addr *)&dest_address.sin_addr.s_addr)) {
        printf("failed to make an in_addr from '%s'\n", BDP_MCAST_ADDR);
        return 0;
    }

    r = sendto(bdp_unicast_socket, buffer, size, 0, (struct sockaddr *)&dest_address, sizeof(struct sockaddr_in));
    if (r == -1) {
        printf("failed to send packet to '%s:%d': %s\n", BDP_MCAST_ADDR, BDP_MCAST_PORT, strerror(errno));
        return 0;
    }
    if (r < size) {
        printf("short write while sending packet to '%s:%d': %s\n", BDP_MCAST_ADDR, BDP_MCAST_PORT, strerror(errno));
        return 0;
    }


    return 1;
}

