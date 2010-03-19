
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "config.h"

#ifndef _WIN32
#error "THIS FILE SHOULD BE USED ONLY BY WINDOWS-TYPE BUILDS"
#endif


#include "../bip-socket-api.h"
#include "../bip-itc.h"

#include <glib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2tcpip.h>

/*
 * The Windows msg queue uses 2 bi-directional
 * inet sockets, 'user' and 'thread'
 *
 * The 'user' socket is only accessed by the user thread, and 
 * the 'thread' socket is only used by the spawned thread.
 *
 * So to write 'to_user', write on the 'thread' socket.
 * to write 'from_user', write on the 'user' socket
 */ 

// Create 2 tcp sockets that are connected to each other
int bip_msg_queue_init(bip_msg_queue_t *q) {
    int r;

    int server_sock, client_sock, accept_sock;

    struct sockaddr_in address = {0};

    address.sin_family = AF_INET;
    address.sin_port = 0;
    address.sin_addr.S_un.S_addr = htonl(0x7F000001);


    // Create listening socket for user thread 
    server_sock = bip_socket_tcp();
    if(server_sock < 0 ) return -1;

    r = bind(server_sock, (struct sockaddr*)&address, sizeof(address));
    if ( r != 0 ) {
        int sock_err = WSAGetLastError();
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                "%s: Error binding for inter-thread pipe: %d", 
                __FUNCTION__,
                sock_err);

        goto fail1;
    }

    r = listen(server_sock, 1);
    if ( r != 0 ) {
        int sock_err = WSAGetLastError();
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                "%s: Error listening on inter-thread pipe: %d", 
                __FUNCTION__,
                sock_err);
        goto fail1;
    }

    // Create connecting socket for bip thread
    client_sock = bip_socket_tcp();
    if(client_sock < 0 ) goto fail1;

    memset(&address, 0, sizeof(address));
    int addr_len = sizeof(address);
    getsockname(server_sock, (struct sockaddr *)&address, &addr_len);
    if ( r != 0 ) {
        int sock_err = WSAGetLastError();
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                "%s: Error getting bound port on inter-thread pipe: %d", 
                __FUNCTION__,
                sock_err);
        goto fail2;
    }


    r = bip_socket_set_blocking(client_sock, 0);
    if ( r < 0 ) goto fail2;

    int connect_err;
    while(connect(client_sock, (struct sockaddr*)&address, addr_len) < 0 ) {
        connect_err = WSAGetLastError();
        if ( connect_err == WSAEINPROGRESS ) {
            continue;
        }

        break; // This won't finish until accept() returns on server_sock
    }

    accept_sock = accept(server_sock, NULL, NULL);
    if ( accept_sock < 0 ) {
        int sock_err = WSAGetLastError();
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                "%s: Error accepting connection on inter-thread pipe: %d", 
                __FUNCTION__,
                sock_err);
        goto fail2;
    }


    fd_set errors, writers;
    FD_ZERO(&errors);
    FD_ZERO(&writers);
    FD_SET(client_sock, &errors);
    FD_SET(client_sock, &writers);


    r = select(client_sock+1, NULL, &writers, &errors, NULL); 
    if(r != 1) {
        int sock_err = WSAGetLastError();
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                "%s: Error selecting connect socket on inter-thread pipe: %d", 
                __FUNCTION__,
                sock_err);
        goto fail2;
    }

    r = bip_socket_set_blocking(client_sock, 1);
    if ( r < 0 ) goto fail2;

    q->user = accept_sock;
    q->thread = client_sock;

    return 0;

fail2:
    closesocket(client_sock);
fail1:
    closesocket(server_sock);

    return -1;
}

void bip_msg_queue_ref(bip_msg_queue_t *q) {

}


void bip_msg_queue_unref(bip_msg_queue_t *q) {
    if(q->user >= 0) {
        int r = closesocket(q->user);
        if ( r < 0 ) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s: error closing pipe %d", __FUNCTION__, WSAGetLastError());
        }

        q->user = -1;
    }

    if(q->thread >= 0) {
        int r = closesocket(q->thread);
        if ( r < 0 ) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s: error closing pipe %d", __FUNCTION__, WSAGetLastError());
        }
        q->thread = -1;
    }
}

int bip_msg_queue_close(bip_msg_queue_t *q, bip_msg_queue_direction_t dir) {
    int *fd;
    if(dir == BIP_MSG_QUEUE_TO_USER){
        fd = &q->thread;
    } else {
        fd = &q->user;
    }

    if(*fd >= 0) {
        int r = shutdown(*fd, SD_SEND);
        if ( r < 0 ) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s: error closing pipe %d", __FUNCTION__, WSAGetLastError());
            return r;
        }
        *fd = -1;
    }
    return 0;
}

int bip_msg_queue_push(bip_msg_queue_t *q, bip_msg_queue_direction_t dir, cal_event_t * event) {
    int r;

    int fd;
    if(dir == BIP_MSG_QUEUE_TO_USER){
        fd = q->thread;
    } else {
        fd = q->user;
    }

    r = send(fd, (char*)&event, sizeof(cal_event_t*), 0);
    if ( r < 0 ) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            "%s: error writing to pipe: %d", __FUNCTION__, WSAGetLastError());
        return -1;
    }
    if ( r != sizeof(cal_event_t*) ) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            "%s: short write to pipe", __FUNCTION__);
        return -1;
    }

    return 0;
}


int bip_msg_queue_pop(bip_msg_queue_t *q, bip_msg_queue_direction_t dir, cal_event_t ** event) {
    int r;

    int fd;
    if(dir == BIP_MSG_QUEUE_TO_USER){
        fd = q->user;
    } else {
        fd = q->thread;
    }

    r = recv(fd, (char*)event, sizeof(cal_event_t*), 0);

    if (r == sizeof(cal_event_t*) ) {
        return 0;
    } 
    
    if (r == 0 ) {
        *event = NULL;
        return 1;
    }

    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s: error reading from pipe: %d", 
                __FUNCTION__, WSAGetLastError());
    }

    *event = NULL;
    return -1;

}

#if 0
int bip_msg_queue_set_blocking(bip_msg_queue_t q, int blocking) {
    long flags = 0;
    flags = fcntl(cal_server_mdnssd_bip_fds_to_user[0], F_GETFL, 0);
    if (flags < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "init: cannot get flags for cal_fd: %d", WSAGetLastError());
        return -1;
    }

    if(blocking) {
        flags &= ~O_NONBLOCK;
    } else {
        flags |= O_NONBLOCK;
    }

    r = fcntl(cal_server_mdnssd_bip_fds_to_user[0], F_SETFL, O_NONBLOCK);
    if (r != 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "init: cannot set cal_fd flags: %d", WSAGetLastError());
        return -1;
    }


    return 0;
}
#endif

