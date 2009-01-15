
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
 * Send the RESPONDER message to the UWB.exe program.
 */
int uwb_read(int fd, char* buffer) 
{
	int bytes = 0;

	memset(buffer, '\0', BUFFER_SZ);

	bytes = recvfrom(fd, buffer, BUFFER_SZ, 0, 0, 0);
	
	if (bytes < 0) {
		g_warning("\tuwb_read(): recvfrom failed, %s", strerror(errno));
		g_error("\tuwb_read(): exiting...");
	}

	return bytes;
}

