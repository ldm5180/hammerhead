
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <glib.h>

#include "uwb.h"


/*
 * Establish the connection to the UWB.exe code
 */
int uwb_connect(const int uwb_port) 
{
	int uwb_fd = -1;

	// Prepare the uwb address.
	memset((char *) &uwb_address, '\0', sizeof(uwb_address));
	uwb_address.sin_family = AF_INET;
	uwb_address.sin_addr.s_addr = INADDR_ANY;
	uwb_address.sin_port = htons(uwb_port);

	// Create the socket.
	if ((uwb_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		g_warning("uwb_connect(): cannot create local socket: %s",
			strerror(errno));

		return -1;
	}

	if (bind(uwb_fd, (struct sockaddr *) &uwb_address, 
             sizeof(struct sockaddr_in)) < 0) {
		g_warning("uwb_connect(): failed to bind(), %d, %s", 
			errno, strerror(errno));

		close(uwb_fd);

		return -1;
	}

	return uwb_fd;
}
