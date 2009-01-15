
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
int radio_connect(const char *radio_ip, const int radio_port) 
{
	int radio_fd = -1;

	// Prepare the radio address.
	memset((char *) &uwb_address, '\0', sizeof(uwb_address));
	uwb_address.sin_family = AF_INET;
	uwb_address.sin_addr.s_addr = inet_addr(radio_ip);
	uwb_address.sin_port = htons(radio_port);

	if (inet_aton(radio_ip, &uwb_address.sin_addr) == 0) {
		g_warning("radio_connect(): invalid radio ip: '%s'", radio_ip);

		return -1;
	}

	// Create the socket.
	if ((radio_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		g_warning("radio_connect(): cannot create local socket: %s",
			strerror(errno));

		return -1;
	}

	if (bind(radio_fd, (struct sockaddr *) &uwb_address, 
             sizeof(struct sockaddr_in)) < 0) {
		g_warning("radio_connect(): failed to bind(), %d, %s", 
			errno, strerror(errno));

		close(radio_fd);

		return -1;
	}

	return radio_fd;
}
