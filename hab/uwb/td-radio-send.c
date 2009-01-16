
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <glib.h>

#include "td-radio.h"


/*
 * Send the RESPONDER message to the radio.
 */
int radio_send(int fd) 
{
	char buffer[10] = "RESPONDER";
	int buffer_sz = sizeof(buffer);

	int sent = 0;

	sent = sendto(fd, buffer, buffer_sz, 0,
		(struct sockaddr *) &radio_address, sizeof(radio_address));
		
	if (sent < 0) {
		g_warning("radio_connect(): Failed to send responder message, %s", 
			strerror(errno));

		close(fd);
	}

	return fd;
}
