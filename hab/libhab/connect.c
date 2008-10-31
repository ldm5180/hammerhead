
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

#if defined(LINUX) || defined(MAC_OSX)
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




static const char *libhab_get_program_name(void) {
#if defined(LINUX) || defined(MAC_OSX)
    static char program_name[500];

    int r;
    int fd;
    char *tmp;

    fd = open("/proc/self/cmdline", O_RDONLY);
    if (fd < 0) {
        g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "hab_connect_to_nag(): error opening /proc/self/cmdline (%s), oh well",
            strerror(errno)
        );
        return "unknown";
    }


    r = read(fd, program_name, sizeof(program_name) - 1);
    close(fd);

    if (r <= 0) {
        g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "hab_connect_to_nag(): error reading /proc/self/cmdline (%s), oh well",
            strerror(errno)
        );
        return "unknown";
    }

    program_name[r] = '\0';  // this is redundant, because the /proc file already contains the NUL, but it makes Coverity happy

    while ((tmp = memchr(program_name, '/', strlen(program_name))) != NULL) {
        int new_len = strlen(tmp+1);
        memmove(program_name, tmp+1, new_len);
        program_name[new_len] = '\0';
    }

    for (tmp = program_name; *tmp != '\0'; tmp ++) {
        if (!isalnum(*tmp) && *tmp != '-') {
            *tmp = '-';
        }
    }

    return program_name;
#endif

#ifdef WINDOWS
    // FIXME
    return "a-windows-program";
#endif
}




int hab_connect(char *hab_type, char *hab_id) {
    char cal_name[BIONET_NAME_COMPONENT_MAX_LEN * 2];
    char hostname[256];


    //
    // it we're already connected to Bionet, we're done
    //

    if (libhab_cal_fd != -1) return libhab_cal_fd;


    //
    // If we get here we need to actually open the connection.
    //


    //
    // If the HAB developer did not specify the HAB-Type using, we print a
    // warning and fall back to using the program name.
    //

    if (hab_type == NULL) {
        // get the program name
        hab_type = (char *)libhab_get_program_name();
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "hab_connect_to_nag(): no HAB-Type specified, using program name '%s'", hab_type);
    }


    //
    // If the HAB developer did not specify the HAB-ID we print a warning
    // and fall back to using the hostname.
    //

    if (hab_id == NULL) {
        char *p;
        int r;

        r = gethostname(hostname, sizeof(hostname));
        if (r < 0) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_connect_to_nag(): no HAB-ID specified and could not get hostname: %s", strerror(errno));
            return -1;
        }

        hab_id = hostname;

        for (p = hab_id; *p != '\0'; p++) {
            if (!isalnum(*p) && (*p != '-')) *p = '-';
        }

        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "hab_connect_to_nag(): no HAB-ID specified, using hostname '%s'", hab_id);
    }


    sprintf(cal_name, "%s.%s", hab_type, hab_id);

    libhab_cal_fd = cal_server.init(cal_name, libhab_cal_callback);
    if (libhab_cal_fd == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_connect(): error initializing CAL");
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

#if defined(LINUX) || defined(MAC_OSX)
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
#if defined(LINUX) || defined(MAC_OSX)
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

#if defined(LINUX) || defined(MAC_OSX)
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

