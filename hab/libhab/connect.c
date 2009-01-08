
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined(LINUX) || defined(MACOSX)
    #include <netdb.h>
    #include <pwd.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
#endif

#ifdef WINDOWS
    #include <winsock.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>

#include <glib.h>

#include "cal-server.h"

#include "libhab-internal.h"
#include "hardware-abstractor.h"


int hab_connect(bionet_hab_t *hab) {
    const char *cal_name;


    // 
    // sanity checks
    //

    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_connect(): no HAB specified");
        return -1;
    }


    //
    // it we're already connected to Bionet, we're done
    //

    if (libhab_cal_fd != -1) return libhab_cal_fd;


    //
    // If we get here we need to actually open the connection.
    //

    // record this hab
    libhab_this = hab;

    cal_name = bionet_hab_get_name(libhab_this);
    if (cal_name == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_connect(): error getting HAB name");
        libhab_this = NULL;
        return -1;
    }

    libhab_cal_fd = cal_server.init("bionet", cal_name, libhab_cal_callback, libhab_cal_topic_matches);
    if (libhab_cal_fd == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_connect(): error initializing CAL");
        libhab_this = NULL;
        return -1;
    }


#if 0

    //
    // If the server dies or the connection is lost somehow, writes will
    // cause us to receive SIGPIPE, and the default SIGPIPE handler
    // terminates the process.  So we need to change the handler to ignore
    // the signal, unless the process has explicitly changed the action.
    //
    // FIXME: how to deal with this in Windows?
    //

#if defined(LINUX) || defined(MACOSX)
    {
        int r;
        struct sigaction sa;

        r = sigaction(SIGPIPE, NULL, &sa);
        if (r < 0) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_connect_to_nag(): error getting old SIGPIPE sigaction: %s", strerror(errno));
            return -1;
        }

        if (sa.sa_handler == SIG_DFL) {
            sa.sa_handler = SIG_IGN;
            r = sigaction(SIGPIPE, &sa, NULL);
            if (r < 0) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_connect_to_nag(): error setting SIGPIPE sigaction to SIG_IGN: %s", strerror(errno));
                return -1;
            }
        }
    }
#endif


#ifdef WINDOWS
    {
        WORD wVersionRequested;
        WSADATA wsaData;
        int err;

        wVersionRequested = MAKEWORD( 2, 2 );

        err = WSAStartup( wVersionRequested, &wsaData );
        if ( err != 0 ) {
            /* Tell the user that we could not find a usable */
            /* WinSock DLL.                                  */
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_connect_to_nag(): cannot find a usable WinSock DLL");
            return -1;
        }

        /* Confirm that the WinSock DLL supports 2.2.*/
        /* Note that if the DLL supports versions greater    */
        /* than 2.2 in addition to 2.2, it will still return */
        /* 2.2 in wVersion since that is the version we      */
        /* requested.                                        */

        if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 ) {
            /* Tell the user that we could not find a usable */
            /* WinSock DLL.                                  */
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_connect_to_nag(): cannot find a version 2.2 WinSock DLL");
            WSACleanup( );
            return -1; 
        }

        /* The WinSock DLL is acceptable. Proceed. */
    }
#endif


    //
    // get the hostent for the server
    //

    if (libhab_nag_hostname == NULL) {
        hostname = "localhost";
    } else {
        hostname = libhab_nag_hostname;
    }

    server_host = gethostbyname(hostname);

    if (server_host == NULL) {
#if defined(LINUX) || defined(MACOSX)
        const char *error_string;
        error_string = hstrerror(h_errno);
#endif

#ifdef WINDOWS
        char error_string[100];
        sprintf(error_string, "%d", WSAGetLastError());
#endif

        g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "hab_connect_to_server(): gethostbyname(\"%s\"): %s",
            hostname,
            error_string
        );
        return -1;
    }


    //
    // create the socket
    //

    libhab_nag_nxio->socket = socket(AF_INET, SOCK_STREAM, 0);

#if defined(LINUX) || defined(MACOSX)
    if (libhab_nag_nxio->socket < 0) {
        g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "hab_connect_to_server(): cannot create local socket: %s",
            strerror(errno)
        );
        return -1;
    }
#endif

#ifdef WINDOWS
    if (libhab_nag_nxio->socket == INVALID_SOCKET) {
        g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "hab_connect_to_server(): cannot create local socket, error %d",
            WSAGetLastError()
        );
        return -1;
    }
#endif


    //  This makes it to the underlying networking code tries to send any
    //  buffered data, even after we've closed the socket.
    {
        struct linger linger;

        linger.l_onoff = 1;
        linger.l_linger = 60;
        if (setsockopt(libhab_nag_nxio->socket, SOL_SOCKET, SO_LINGER, (char *)&linger, sizeof(linger)) != 0) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_connect_to_nag(): WARNING: cannot make socket linger: %s", strerror(errno));
        }
    }


    // prepare the server address
    memset((char *)&server_address, '\0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)(*server_host->h_addr_list)));
    server_address.sin_port = g_htons(libhab_nag_port);


    // connect to the server
    if (connect(libhab_nag_nxio->socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "hab_connect_to_nag(): failed to connect to server %s:%d: %s",
            server_host->h_name,
            libhab_nag_port,
            strerror(errno)
        );

        hab_disconnect_from_nag();

        return -1;
    }



#endif


    return libhab_cal_fd;
}




#if 0

int hab_is_connected(void) {
    if (libhab_nag_nxio == NULL) {
        return 0;
    }

    if (libhab_nag_nxio->socket < 0) {
        return 0;
    }

    return 1;
}

#endif

