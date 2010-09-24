
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "bps/bps_socket.h"

static void usage(const char * argv0) {
    printf("Usage: %s <src_eid>\n", argv0);
}

int main(int argc, char* argv[]) {
    int r;

    if(argc < 2) {
        usage(argv[0]);
        exit(1);
    }

    printf("Starting %s...\n", argv[0]);

    int fd = bps_socket(0,0,0);
    if(fd < 0) {
        printf("Error creating bps socket: %s\n", strerror(errno));
        exit(1);
    }

    struct bps_sockaddr addr;
    strcpy(addr.uri, argv[1]);
    r = bps_bind(fd, &addr, sizeof(struct bps_sockaddr));
    if( r ) {
        fprintf(stderr, "Error binding bps socket to '%s': %s\n", 
               argv[1], strerror(errno));
        exit(1);
    }


    int running = 1;
    while(running) {
        char msg[255];

        struct bps_sockaddr remote_addr;
        socklen_t addrlen = sizeof(remote_addr);
        ssize_t bytes = bps_recvfrom(fd, msg, sizeof(msg), MSG_TRUNC, 
                &remote_addr, &addrlen);
        if(bytes >= sizeof(msg)) {
            fprintf(stderr, "Incomming message trucated from %zd to %zd bytes\n",
                    bytes, sizeof(msg)-1);
            msg[sizeof(msg)-1] = '\0';
        }

        printf("BDL: %s\n", msg);
        printf("Echoing to %s\n", remote_addr.uri);

        ssize_t bytes_sent = bps_sendto(fd, msg, bytes, 0, 
                &remote_addr, sizeof(struct bps_sockaddr));
        if(bytes_sent < 0) {
            fprintf(stderr, "Error sending msg: %s\n",
                    strerror(errno));
        }
    }



    return 0;
}

