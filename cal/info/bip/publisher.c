
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <netinet/in.h>

#include <sys/socket.h>

#include "cal.h"
#include "bip.h"




int bip_init_publisher(cal_peer_t *this, void (*callback)(cal_event_t *event)) {
    int r;
    int sock;

    struct sockaddr_in my_address;
    socklen_t my_address_len;


    cal_i.publisher_callback = callback;

    cal_peer_set_addressing_scheme(this, CAL_AS_IPv4);


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



    // ok! listen for connections
    // we dont need to bind since listen on an unbound socket defaults to INADDR_ANY and a random port, which is what we want
    r = listen(sock, 20);
    if (r != 0) {
        printf("ERROR: cannot listen on port: %s\n", strerror(errno));
        return -1;
    }

    memset(&my_address, 0, sizeof(my_address));
    my_address_len = sizeof(my_address);
    r = getsockname(sock, (struct sockaddr *)&my_address, &my_address_len);
    if (r != 0) {
        fprintf(stderr, "bip: bip_init_publisher(): cannot get socket port: %s\n", strerror(errno));
        return -1;
    }


    bip_socket = sock;

    this->as.ipv4.socket = sock;
    this->as.ipv4.port = ntohs(my_address.sin_port);

    return bip_socket;
}


void bip_publish(char *topic, void *msg, int size) {
    printf("bip_publish is not implemented yet\n");
}


#if 0
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
#endif


int bip_publisher_read(void) {
    int socket;
    struct sockaddr_in sin;
    socklen_t sin_len;

    cal_event_t *event;


    sin_len = sizeof(struct sockaddr);
    socket = accept(bip_socket, (struct sockaddr *)&sin, &sin_len);
    if (socket < 0) {
        fprintf(stderr, "bip: bip_publisher_read(): error accepting a connection: %s\n", strerror(errno));
        return 0;
    }

    event = cal_event_new(CAL_EVENT_CONNECT);
    if (event == NULL) {
        // an error has been logged already
        close(socket);
        return 1;
    }

    event->peer = cal_peer_new(NULL);
    if (event->peer == NULL) {
        // an error has been logged already
        cal_event_free(event);
        close(socket);
        return 1;
    }

    cal_peer_set_addressing_scheme(event->peer, CAL_AS_IPv4);
    event->peer->as.ipv4.socket = socket;
    event->peer->as.ipv4.port = ntohs(sin.sin_port);

    event->peer->as.ipv4.hostname = strdup(inet_ntoa(sin.sin_addr));
    if (event->peer->as.ipv4.hostname == NULL) {
        fprintf(stderr, "bip: bip_publisher_read(): out of memory\n");
        cal_event_free(event);
        close(socket);
        return 1;
    }


    if (cal_i.publisher_callback != NULL) {
        cal_i.publisher_callback(event);
    }


    cal_event_free(event);

    return 1;
}

