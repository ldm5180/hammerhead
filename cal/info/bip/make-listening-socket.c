
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>


int bip_make_listening_socket(void) {
    int r;
    int sock;


    //
    // create the socket
    //

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("ERROR: cannot create TCP socket: %s\n", strerror(errno));
        return -1;
    }


    // turn on REUSEADDR, so it'll start right back up after dying
    {
        int flag = 1;
        int r;

        r = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*)&flag, sizeof(int));
        if (r < 0) printf("WARNING: ignoring setsockopt SO_REUSEADDR error: %s", strerror(errno));
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


    // ok! listen for connections
    r = listen(sock, 20);
    if (r != 0) {
        printf("ERROR: cannot listen on port: %s\n", strerror(errno));
        return -1;
    }


    return sock;
}

