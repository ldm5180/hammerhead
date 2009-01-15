
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

#include "td-radio.h"
#include "td-radio-scan.h"


/*
 * Send the RESPONDER message to the radio.
 */
int radio_read(int fd, char* buffer) 
{
	bool debug = true;
	int bytes = 0;

	memset(buffer, '\0', BUFFER_SZ);

	if (debug) {
		g_message("radio_read(): enter");
		g_message("\ts_addr = %d", radio_address.sin_addr.s_addr);
		g_message("\tsin_port = %d", radio_address.sin_port);
	}

	socklen_t socklen = sizeof(radio_address);

	bytes = recvfrom(fd, buffer, BUFFER_SZ, 0,
		(struct sockaddr *) &radio_address, &socklen);
	
	if (debug) {
		g_message("bytes = %d", bytes);
	}


	if (bytes < 0) {
		g_warning("\tradio_read(): recvfrom failed, %s", strerror(errno));
		g_message("\tradio_read(): exit");
	}

	if (debug) {
		g_message("radio_read(): exit");
	}

	return bytes;
}

