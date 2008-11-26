
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <string.h>
#include <stdlib.h>

#if defined(LINUX) || defined(MACOSX)
    #include <netinet/in.h>
    #include <sys/socket.h>
#endif

#ifdef WINDOWS
    #include <winsock2.h>
#endif

#include <glib.h>

#include "bionet-data-manager.h"




int make_listening_socket(int port) {
    int sock;
    struct sockaddr_in my_address;

    int r;


    // create the socket
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        g_critical("ERROR: cannot create TCP socket: %s\n", strerror(errno));
        exit(1);
    }


    // turn on REUSEADDR, so it'll start right back up after dying
    {
        int flag = 1;
        int r;

        r = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*)&flag, sizeof(int));
        if (r == -1) g_warning("WARNING: ignoring setsockopt SO_REUSEADDR error: %s", strerror(errno));
    }


    // bind to the local port
    memset(&my_address, 0, sizeof(my_address));
    my_address.sin_family = AF_INET;
    my_address.sin_port = g_htons(port);
    my_address.sin_addr.s_addr = g_htonl(INADDR_ANY);

    r = bind(sock, (struct sockaddr *)&my_address, sizeof(my_address));
    if (r != 0) {
        g_critical("ERROR: cannot bind port %d: %s\n", port, strerror(errno));
        exit(1);
    }


    // ok! listen for connections
    r = listen(sock, 20);
    if (r != 0) {
        g_critical("ERROR: cannot listen on port %d: %s\n", port, strerror(errno));
        exit(1);
    }


    return sock;
}


