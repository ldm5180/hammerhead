
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>


int bdp_make_mcast_socket(const char *mcast_addr, uint16_t mcast_port) {
    int r;
    int sock;


    //
    // create the socket
    //

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        printf("ERROR: cannot open UDP socket: %s\n", strerror(errno));
        return -1;
    }


    //
    // join the specified multicast group
    // let the kernel choose the interface for accepting incoming multicast packets 
    //

    {
        struct ip_mreq mreq;

        if (inet_aton(mcast_addr, &mreq.imr_multiaddr) == 0) {
            printf("cannot get multicast address from '%s': %s\n", mcast_addr, strerror(errno));
            return -1;
        }

        mreq.imr_interface.s_addr = INADDR_ANY;

        if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) != 0) {
            printf("cannot join multicast group '%s': %s\n", mcast_addr, strerror(errno));
            return -1;
        }
    }


    //
    // let the kernel choose the interface for outgoing multicast packets based in the routing table
    //

    {
        struct in_addr addr;

        addr.s_addr = INADDR_ANY;

        if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, &addr, sizeof(addr)) != 0) {
            printf("cannot select INADDR_ANY as the IP_MULTICAST_IF: %s\n", strerror(errno));
            return -1;
        }
    }


    //
    // turn on SO_REUSEADDR so we can share this multicast UDP port with other processes
    //

    {
        int flag = 1;

        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) != 0) {
            printf("failed to enable SO_REUSEADDR on mcast socket: %s", strerror(errno));
            return -1;
        }
    }


    //
    // set the TTL to 31, so the multicast packets go all throughout the "site"
    //

    {
        int ttl = 31;

        if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(int)) != 0) {
            printf("failed to set TTL on mcast socket: %s", strerror(errno));
            return -1;
        }
    }


    //
    // multicast loopback is enabled by default
    //


    //
    // bind to the local port
    //

    {
        struct sockaddr_in my_address;

        bzero(&my_address, sizeof(my_address));
        my_address.sin_family = AF_INET;
        my_address.sin_port = htons(mcast_port);
        my_address.sin_addr.s_addr = htonl(INADDR_ANY);

        r = bind(sock, (struct sockaddr *)&my_address, (socklen_t)sizeof(my_address));
        if (r != 0) {
            printf("cannot bind port %d: %s\n", mcast_port, strerror(errno));
            return -1;
        }
    }


    return sock;
}

