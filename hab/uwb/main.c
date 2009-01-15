/*
 * Written to interface with the Time Domain radio.
 *
 * G.L. Grobe
 * Ported to Bionet 2.1 by Shea Williams
 *
 */

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <glib.h>

#include <hardware-abstractor.h>

#include "uwb.h"

bionet_hab_t *uwb_hab;
struct sockaddr_in uwb_address;

static GOptionEntry entries[] = {
	{"ip", 'a', 0, G_OPTION_ARG_STRING, &ip, "IP address of radio", NULL},
	{"port", 'p', 0, G_OPTION_ARG_INT, &port, "Radio data port", NULL}, 
	{"timeout", 't', 0, G_OPTION_ARG_INT, &timeout, 
		"Seconds of absense before reporting no data.", NULL}, 
	{NULL}
};

int main(int argc, char *argv[]) {
	int radio_fd = -1;
	int hab_fd = -1;
	int bytes = 0;
	char *buffer;

	// Write the error messages to the correct location.
	bionet_log_context_t log_context = {
		destination: BIONET_LOG_TO_STDOUT,
		log_limit: G_LOG_LEVEL_INFO
	};

	g_log_set_default_handler(bionet_glib_log_handler, &log_context);

	// Parse command-line arguments.
	GError *error = NULL;
	GOptionContext *context;

	context = g_option_context_new (
		"A radio interface for the Time Domain hardware.");

	g_option_context_add_main_entries(context, entries, NULL);

	int r = g_option_context_parse(context, &argc, &argv, &error);

	if (! r) {
		g_warning("Error parsing.");
		exit(1);
	}

	if (ip == NULL) {
		g_warning("Radio IP not specified.");
		exit(1);
	}

	if (port == 0) {
		g_warning("Radio port not secified.");
		exit(1);
	}

	uwb_hab = bionet_hab_new("TD-P210", NULL);
	hab_fd = hab_connect(uwb_hab);	
	if (hab_fd == -1) {
	  g_error("Cannot connect to Bionet, quitting.");
	  exit(1);
	}

	make_shutdowns_clean();

	buffer = calloc(BUFFER_SZ, sizeof(unsigned int));

	if (buffer == NULL) {
		g_warning("could not allocation receive buffer.");

		return -1;
	}

	fd_set reader;
	struct timeval tv;

	while (1) {

        int max;

		// Likewise, make sure we're connected to the radio.
		if (radio_fd < 0) {
			radio_fd = radio_connect(ip, port);

			if (radio_fd < 0) {
				g_warning("Connection failed.");
				return -1;
			}

			g_message("Connected to the TD-P210.");
			g_message("Radio IP address: %s", ip);
			g_message("Radio port: %d", port);
			g_message("Timeout: %d", timeout);
		}

		FD_ZERO(&reader);
		FD_SET(radio_fd, &reader);
		FD_SET(hab_fd, &reader);
		max = (hab_fd > radio_fd ? hab_fd : radio_fd);

		tv.tv_sec = timeout;
		tv.tv_usec = 0;

		int ready = select(max + 1, &reader, NULL, NULL, &tv);

		if (ready == -1) {
			g_warning("error with select(): %s", strerror(errno));
		}
		else if (ready >= 1) {

		  if (FD_ISSET(radio_fd, &reader)) {
			  bytes = radio_read(radio_fd, buffer);

			  if (bytes < 0)  {
				  g_message("radio_read() returned: %d.", bytes);
				  continue;
			  }
		  } 
		  if (FD_ISSET(hab_fd, &reader)) {
			hab_read();
		  }
		}
		else {
			g_message("No data for the last %d seconds.", timeout);
		}
	}

	return 0;
}

