
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <string.h>



#if defined(LINUX) || defined(MACOSX)
    #include <netdb.h>
    #include <netinet/tcp.h>
#endif

#ifdef WINDOWS
    #include <winsock.h>
    typedef int socklen_t;
#endif

#include <glib.h>

#include "bionet-data-manager.h"




void keepalive(int socket, int idle, int count, int interval) {
    int r;
    int opt;

#ifdef LINUX
    socklen_t optsize;
#endif

    opt = 1;
    r = setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, (void*)&opt, sizeof(opt));
    if (r < 0) {
        g_warning("error enabling socket keepalive: %s", strerror(errno));
    }
#ifdef LINUX
    r = setsockopt(socket, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(idle));
    if (r < 0) {
        g_warning("error setting socket keepalive idle time to %d: %s", opt, strerror(errno));
    }
    opt = -1;
    optsize = sizeof(opt);
    r = getsockopt(socket, IPPROTO_TCP, TCP_KEEPIDLE, &opt, &optsize);
    if (r < 0) {
        g_warning("error getting socket keepalive idle time: %s", strerror(errno));
    }

    r = setsockopt(socket, IPPROTO_TCP, TCP_KEEPCNT, &count, sizeof(count));
    if (r < 0) {
        g_warning("error setting socket keepalive probe count to %d: %s", opt, strerror(errno));
    }
    opt = -1;
    optsize = sizeof(opt);
    r = getsockopt(socket, IPPROTO_TCP, TCP_KEEPCNT, &opt, &optsize);
    if (r < 0) {
        g_warning("error getting socket keepalive probe count: %s", strerror(errno));
    }

    r = setsockopt(socket, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));
    if (r < 0) {
        g_warning("error setting socket keepalive probe interval to %d: %s", opt, strerror(errno));
    }
    opt = -1;
    optsize = sizeof(opt);
    r = getsockopt(socket, IPPROTO_TCP, TCP_KEEPINTVL, &opt, &optsize);
    if (r < 0) {
        g_warning("error getting socket keepalive probe interval: %s", strerror(errno));
    }
#endif /* LINUX */
}

