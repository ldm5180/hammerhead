
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined(LINUX) || defined(MACOSX)
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/types.h>
    #include <sys/socket.h>
#endif

#ifdef WINDOWS
    #include <winsock.h>
    typedef int socklen_t;
#endif

#include <glib.h>

#include "bionet-data-manager.h"




int client_connecting_handler(GIOChannel *listening_ch, GIOCondition condition, gpointer listening_fd_as_pointer) {
    int listening_fd = GPOINTER_TO_INT(listening_fd_as_pointer);

    struct sockaddr_in addr;
    socklen_t addrlen;

#if 0
    char id[100];
#endif

    client_t *new_client;

    int socket;

    int idle = 60;
    int count = 6;
    int interval = 10; 


    addrlen = sizeof(addr);
    socket = accept(listening_fd, (struct sockaddr *)&addr, &addrlen);
    if (socket < 0) {
        g_critical("error connecting Client: %s", strerror(errno));
        return TRUE;
    }

    keepalive(socket, idle, count, interval);


    new_client = (client_t *)calloc(1, sizeof(client_t));
    if (new_client == NULL) {
        g_critical("out of memory while connecting new client!");
        close(socket);
        g_usleep(100*1000);
        return TRUE;
    }

    new_client->fd = socket;


#if 0
    snprintf(
        id,
        sizeof(id) - 1,
        "%s:%hu",
        inet_ntoa(addr.sin_addr),
        g_htons(addr.sin_port)
    );
    id[sizeof(id) - 1] = '\0';

    new_client->id = strdup(id);

    new_client->known_habs = g_hash_table_new(g_direct_hash, g_direct_equal);
    new_client->known_nodes = g_hash_table_new(g_direct_hash, g_direct_equal);
    new_client->known_resources = g_hash_table_new(g_direct_hash, g_direct_equal);

    clients = g_slist_append(clients, new_client);
#endif


#if defined(LINUX) || defined(MACOSX)
    new_client->ch = g_io_channel_unix_new(new_client->fd);
#endif

#ifdef WINDOWS
    new_client->ch = g_io_channel_win32_new_socket(new_client->fd);
#endif

    if (new_client->ch == NULL) {
        g_critical("out of memory while making channel for connecting client!");
        close(new_client->fd);
        free(new_client);
        g_usleep(100*1000);
        return TRUE;
    }

    g_io_add_watch(new_client->ch, (G_IO_IN | G_IO_ERR | G_IO_HUP | G_IO_NVAL), (GIOFunc)client_readable_handler, new_client);


    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "accepted connection from %s:%hu", inet_ntoa(addr.sin_addr), g_htons(addr.sin_port));

    return TRUE;
}


