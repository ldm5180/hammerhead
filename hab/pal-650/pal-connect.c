
//
// Copyright (C) 2008-2009, Gary Grobe and the Regents of the University of Colorado.
//


#include <errno.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <glib.h>

#include "pal-650-hab.h"

/*
 * Here we establish a connection the to PAL-650 on, what should be, 
 * port 5117, and return the file descriptor.
 */

int pal_connect(const char *pal_ip, const int pal_port) {
    int pal_fd;
    struct sockaddr_in pal_address;

   	/*
	 * If the server dies or the connection is lost somehow, writes will
	 * cause us to receive SIGPIPE, and the default SIGPIPE handler
	 * terminates the process.  So we need to change the handler to ignore
	 * the signal, unless the process has explicitly changed the action.
	 */

    {
        int r;
        struct sigaction sa;

        r = sigaction(SIGPIPE, NULL, &sa);

        if (r < 0) {
            g_warning(
				"pal_connect(): error getting old SIGPIPE sigaction: %s", 
				strerror(errno));

            return -1;
        }

        if (sa.sa_handler == SIG_DFL) {
            sa.sa_handler = SIG_IGN;
            r = sigaction(SIGPIPE, &sa, NULL);

            if (r < 0) {
                g_warning("pal_connect(): error setting SIGPIPE sigaction to SIG_IGN: %s", strerror(errno));
                return -1;
            }
        }
    }

	/*
	 * Prepare the PAL address.
	 */
    memset((char *)&pal_address, '\0', sizeof(pal_address));
    pal_address.sin_family = AF_INET;
    pal_address.sin_port = g_htons(pal_port);

    {
        int r;

        r = inet_aton(pal_ip, &pal_address.sin_addr);
        if (r == 0) {
            g_warning("pal_connect(): invalid PAL-650 IP '%s'", pal_ip);
            return -1;
        }
    }

	/*
	 * Create the socket.
	 */
    if ((pal_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        g_warning("pal_connect(): cannot create local socket: %s", 
			strerror(errno));

        return -1;
    }

	/*
	 * Connect to the PAL.
	 */
    if (connect(pal_fd, (struct sockaddr *)&pal_address, sizeof(pal_address))
			< 0) {

        g_warning("pal_connect(): failed to connect to PAL at %s:%d: %s", 
			pal_ip, pal_port, strerror(errno));

        close(pal_fd);

        return -1;
    }

	/*
	 * PAL does not require a login.
    {
        char login[] = "?????\n";
        char passwd[] = "password\n";
        int r;

        r = write(pal_fd, login, strlen(login));
        if (r != strlen(login)) {
            g_warning("error sending login to pal");
            close(pal_fd);

            return -1;
        }

        r = write(pal_fd, passwd, strlen(passwd));

        if (r != strlen(passwd)) {
            g_warning("error sending passwd to PAL");
            close(pal_fd);

            return -1;
        }
    }
	*/

    return pal_fd;
}

