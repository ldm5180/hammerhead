
#include <arpa/inet.h>
#include <glib.h>
#include <stdio.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

static char buffer[BUFFER_SIZE];

// Parsec informaiton comes in the format:
// <id> <range[m]> <temperature[C]>

void read_parsec(int fd) {
  ssize_t size;
  unsigned int id, scanned;
  float range;
  float temp;

  size = recvfrom(fd, buffer, BUFFER_SIZE, 0, 0, 0);

  scanned = sscanf(buffer, "%ud %f %f", &id, &range, &temp);

  if (scanned != 3) {
	buffer[BUFFER_SIZE - 1] = '\0';
	g_warning("Couldn't properly scan message sent: %s", buffer);
	return;
  }

  
}
