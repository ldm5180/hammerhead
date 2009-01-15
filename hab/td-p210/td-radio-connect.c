
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


/*
 * Establish the connection to the P210 (or possibly other models) Time Domain 
 * radio.
 */
int radio_connect(const char *radio_ip, const int radio_port) 
{
	bool debug = false;

	int on = 1;
	int radio_fd = -1;

	if (debug) {
		g_message("\tradio_connect(): enter");
		g_message("\tradio_ip: %s", radio_ip);
		g_message("\tradio_port: %d", radio_port);
	}
	
	// Prepare the local address.
	memset((char *) &local_address, '\0', sizeof(struct sockaddr_in));
	local_address.sin_family = AF_INET;
	local_address.sin_addr.s_addr = INADDR_ANY;
	local_address.sin_port = htons(2010);

	//serv_addr.sin_addr.s_addr = inet_addr("www.heise.de");	

	// Prepare the radio address.
	memset((char *) &radio_address, '\0', sizeof(radio_address));
	radio_address.sin_family = AF_INET;
	radio_address.sin_addr.s_addr = inet_addr(radio_ip);
	radio_address.sin_port = htons(radio_port);

	if (inet_aton(radio_ip, &radio_address.sin_addr) == 0) {
		g_warning("radio_connect(): invalid radio ip: '%s'", radio_ip);

		return -1;
	}

	// Create the socket.
	if ((radio_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		g_warning("radio_connect(): cannot create local socket: %s",
			strerror(errno));

		return -1;
	}

	if (bind(radio_fd, (struct sockaddr *) &local_address, 
			sizeof(struct sockaddr_in)) < 0) {
		g_warning("radio_connect(): failed to bind(), %d, %s", 
			errno, strerror(errno));

		close(radio_fd);

		return -1;
	}

	// Set the socket to broadcast.
	if (setsockopt(radio_fd, SOL_SOCKET, SO_BROADCAST, &on, 
			sizeof(int)) < 0) {
		g_warning("radio_connect(): failed to setsockopt(), %d, %s", 
			errno, strerror(errno));

		close(radio_fd);

		return -1;
	}

	// Send the responder message to the radio.
	char buffer[10] = "RESPONDER";
	int buffer_sz = sizeof(buffer);

	if (sendto(radio_fd, buffer, buffer_sz, 0,
			(struct sockaddr *) &radio_address, sizeof(radio_address)) < 0) {
		g_warning("radio_connect(): failed to send responder message, %d, %s", 
			errno, strerror(errno));

		close(radio_fd);

		return -1;
	}

	if (debug) {
		g_message("\tradio_connect(): exit");
	}

	return radio_fd;
}
