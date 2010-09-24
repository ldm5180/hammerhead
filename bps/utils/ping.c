
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "bps/bps_socket.h"

static void usage(const char * argv0) {
    printf("Usage: %s <src_eid> <dst_eid>\n", argv0);
}

int main(int argc, char* argv[]) {
    int r;

    if(argc < 3) {
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
        printf("Error binding bps socket to '%s': %s\n", 
               argv[1], strerror(errno));
        exit(1);
    }

    int running = 1;
    while(running) {
        char msg_out[] = "This is the message";
        char msg_in[255];

        struct bps_sockaddr remote_addr;
        socklen_t addrlen = sizeof(remote_addr);
        strcpy(remote_addr.uri, argv[2]);

        ssize_t bytes_sent = bps_sendto(fd, msg_out, sizeof(msg_out), 0, 
                &remote_addr, sizeof(struct bps_sockaddr));
        if(bytes_sent < 0) {
            fprintf(stderr, "Error sending msg: %s\n",
                    strerror(errno));
        }


        ssize_t bytes = bps_recvfrom(fd, msg_in, sizeof(msg_in), MSG_TRUNC, 
                &remote_addr, &addrlen);
        if(bytes >= sizeof(msg_in)) {
            fprintf(stderr, "Incomming message trucated from %zd to %zd bytes\n",
                    bytes, sizeof(msg_in)-1);
            msg_in[sizeof(msg_in)-1] = '\0';
        }

        printf("%s: %s\n", remote_addr.uri, msg_in);

        //sleep(1);

    }




    return 0;
}

