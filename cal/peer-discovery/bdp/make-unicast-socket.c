
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>


int bdp_make_unicast_socket(void) {
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
    // bind to a random port
    //

    {
        struct sockaddr_in my_address;

        bzero(&my_address, sizeof(my_address));
        my_address.sin_family = AF_INET;
        my_address.sin_port = 0;  // 0 means "let the kernel pick a port", and we dont need htons for 0
        my_address.sin_addr.s_addr = htonl(INADDR_ANY);

        r = bind(sock, (struct sockaddr *)&my_address, (socklen_t)sizeof(my_address));
        if (r != 0) {
            printf("cannot bind: %s\n", strerror(errno));
            return -1;
        }
    }


    return sock;
}

