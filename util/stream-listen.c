
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


#if defined(LINUX) || defined(MACOSX)
    #include <netinet/in.h>
    #include <sys/socket.h>
#endif

#ifdef WINDOWS
    #include <winsock.h>
#endif


#include <glib.h>

#include "bionet-util.h"




int bionet_stream_listen(bionet_stream_t *stream) {
    struct sockaddr_in my_address;
    int my_socket;

    int r;


    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_listen(): NULL stream passed in");
        return -1;
    }

    // create the socket
    my_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (my_socket == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_listen(): cannot create TCP socket: %s", strerror(errno));
        return -1;
    }


    // bind to a random port on all network interfaces
    memset(&my_address, 0, sizeof(my_address));
    my_address.sin_family = AF_INET;
    my_address.sin_port = 0;
    my_address.sin_addr.s_addr = g_htonl(INADDR_ANY);

    r = bind(my_socket, (struct sockaddr *)&my_address, sizeof(my_address));
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_socket_listen(): cannot bind: %s", strerror(errno));
        close(my_socket);
        return -1;
    }


    // ok! listen for connections
    r = listen(my_socket, 20);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_socket_listen(): cannot listen: %s", strerror(errno));
        close(my_socket);
        return -1;
    }


    {
        unsigned int size = sizeof(my_address);

        r = getsockname(my_socket, (struct sockaddr *)&my_address, &size);
        if (r < 0) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_socket: cannot get socket address!: %s", strerror(errno));
            close(my_socket);
            return -1;
        }
    }

    stream->port = g_ntohs(my_address.sin_port);

    return my_socket;
}


