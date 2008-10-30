
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <string.h>

#if defined(LINUX) || defined(MAC_OSX)
    #include <netdb.h>
    #include <arpa/inet.h>
#endif

#ifdef WINDOWS
    #include <winsock.h>
#endif

#include <glib.h>

#include "bionet-util.h"


int bionet_stream_connect(bionet_stream_t *stream) {
    struct sockaddr_in server_address;
    struct hostent *server_host;
    int my_socket;

    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_connect(): NULL stream passed in");
        return -1;
    }


    // FIXME: handle 'stream already open' condition


    // get the hostent for the server
    server_host = gethostbyname(stream->host);
    if (server_host == NULL) {
        g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "bionet_stream_connect(): gethostbyname(\"%s\"): %s",
            stream->host,
            strerror(errno)
        );
        return -1;
    }


    // create the socket
    if ((my_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "bionet_stream_connect(): cannot create local socket: %s",
            strerror(errno)
        );
        return -1;
    }


    //  This makes it to the underlying networking code tries to send any
    //  buffered data, even after we've closed the socket.
    {
        struct linger linger;

        linger.l_onoff = 1;
        linger.l_linger = 60;
        if (setsockopt(my_socket, SOL_SOCKET, SO_LINGER, (char *)&linger, sizeof(linger)) != 0) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_connect(): WARNING: cannot make socket linger: %s", strerror(errno));
        }
    }


    // prepare the server address
    memset((char *)&server_address, '\0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)(*server_host->h_addr_list)));
    server_address.sin_port = g_htons(stream->port);


    // connect to the stream
    if (connect(my_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "bionet_stream_connect(): failed to connect to stream at %s:%hu: %s",
            server_host->h_name,
            stream->port,
            bionet_get_network_error_string()
        );

        return -1;
    }

    return my_socket;
}


