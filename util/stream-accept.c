
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#if defined(LINUX) || defined(MAC_OSX)
    #include <sys/socket.h>
#endif

#ifdef WINDOWS
    #include <winsock.h>
#endif

#include "bionet-util.h"


int bionet_stream_accept(bionet_stream_t *stream, int listening_socket) {
    int sock;


    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_accept(): NULL stream passed in!");
        return -1;
    }

    sock = accept(listening_socket, NULL, 0);
    if (sock < 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_accept(): error accepting connection: %s", strerror(errno));
        return -1;
    }

#if defined(LINUX) || defined(MAC_OSX)
    // FIXME
    // this makes the socket non-blocking, but it only works on Linux
    {
	int r;

	r = fcntl(sock, F_SETFL, O_NONBLOCK);
	if (r < 0) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_accept(): error making socket nonblocking: %s", strerror(errno));
	}
    }
#endif

    return sock;
}

