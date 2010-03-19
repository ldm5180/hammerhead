
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#ifndef _WIN32
#error "THIS FILE SHOULD BE USED ONLY BY WINDOWS-TYPE BUILDS"
#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2tcpip.h>

#include "cal-mdnssd-bip.h"

int bip_socket_set_blocking(int s, int blocking) {

    int rc;
    
    u_long arg = !blocking;
    rc  = ioctlsocket(s, FIONBIO, &arg);

    if(rc != 0 ) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
            "%s: error setting socket blocking flag to %d: %d", 
                __FUNCTION__, blocking, WSAGetLastError());
        return -1;
    }

    return 0;

}

int bip_socket_connect(
        int s, 
        const struct sockaddr *address,
        socklen_t address_len,
        const char * peer_name,
        const bip_peer_network_info_t *net)
{
    int r;
    while((r = connect(s, address, address_len)) < 0) {
        int sock_err = WSAGetLastError();
        switch (sock_err) {
            case WSAEWOULDBLOCK:
            case WSAEALREADY:
            case WSAEISCONN:
            case WSAEINVAL: // This is returned if linked w/ winsock.dll or wsock32.dll
                // Use select to find out when done
                return 0;

            case WSAEINPROGRESS:
                // EINTR.. Keep trying
                break;

            default:
            {
                struct sockaddr_in *sin = (struct sockaddr_in *)address;
                g_log(
                    CAL_LOG_DOMAIN,
                    G_LOG_LEVEL_WARNING,
                    "%s: error connecting to peer '%s' at %s:%hu (%s): %d",
                    __FUNCTION__,
                    peer_name,
                    net->hostname,
                    net->port,
                    inet_ntoa(sin->sin_addr),
                    sock_err
                );
                return -1;
            }
        }

    }
    return 0;
}

int bip_socket_listen(
        int s, 
        int backlog)
{
    int r;

    struct sockaddr_in  addr;
    int addr_len = sizeof(addr);

    memset(&addr, 0, addr_len);
    addr.sin_family = AF_INET;
    addr.sin_port = 0;
    addr.sin_addr.S_un.S_addr = INADDR_ANY;

    r = bind(s, (struct sockaddr*)&addr, addr_len);
    if ( r != 0 ) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s: error binding socket ephemeral port: %d", 
            __FUNCTION__, WSAGetLastError());
        return -1;
    }

    r = listen(s, backlog);
    if ( r != 0 ) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s: error listening on socket: %d", 
            __FUNCTION__, WSAGetLastError());
        return -1;
    }
    return 0;
}

int bip_socket_tcp(void) {
    int s;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s: error making socket: %d", 
            __FUNCTION__, WSAGetLastError());
        return -1;
    }

    return s;
}



