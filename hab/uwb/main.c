
// Copyright (c) 2008-2010, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


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

char * security_dir = NULL;

static GOptionEntry entries[] = {
    {"port", 'p', 0, G_OPTION_ARG_INT, &port, "UWB port", NULL}, 
    {"node", 'n', 0, G_OPTION_ARG_STRING, &node_id, "UWB node name", NULL},
    {"security-dir", 's', G_OPTION_FLAG_FILENAME, G_OPTION_ARG_STRING, &security_dir, NULL},
    {"timeout", 't', 0, G_OPTION_ARG_INT, &timeout, "Seconds of absense before reporting no data.", NULL}, 
    {NULL}
};

int main(int argc, char *argv[]) {
	int uwb_fd = -1;
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
		"An interface for the UWB.exe.");

	g_option_context_add_main_entries(context, entries, NULL);

	int r = g_option_context_parse(context, &argc, &argv, &error);

	if (! r) {
		g_warning("Error parsing.");
		exit(1);
	}

	if (node_id == NULL) {
	    g_error("You must pick a node name.  Use -n.");
		exit(1);
	}

	if (port == 0) {
		g_warning("UWB port not secified.");
		exit(1);
	}

	if (security_dir) {
	    if (hab_init_security(security_dir, 1)) {
		g_log("", G_LOG_LEVEL_WARNING, "Failed to initialize security.");
	    }
	}



	uwb_hab = bionet_hab_new("UWB", NULL);
	hab_fd = hab_connect(uwb_hab);	
	if (hab_fd == -1) {
	  g_error("Cannot connect to bionet, quitting.");
	  exit(1);
	}

	buffer = calloc(BUFFER_SZ, sizeof(unsigned int));

	if (buffer == NULL) {
		g_warning("could not allocation receive buffer.");

		return -1;
	}

	fd_set reader;
	struct timeval tv;

	while (1) {

        int max;

		// Likewise, make sure we're connected to UWB.exe.
		if (uwb_fd < 0) {
			uwb_fd = uwb_connect(port);

			if (uwb_fd < 0) {
				g_warning("Connection failed.");
				return -1;
			}

			g_message("Connected to the UWB processor.");
			g_message("UWB port: %d", port);
			g_message("Timeout: %d", timeout);
		}

		FD_ZERO(&reader);
		FD_SET(uwb_fd, &reader);
		FD_SET(hab_fd, &reader);
		max = (hab_fd > uwb_fd ? hab_fd : uwb_fd);

		tv.tv_sec = timeout;
		tv.tv_usec = 0;

		int ready = select(max + 1, &reader, NULL, NULL, &tv);

		if (ready == -1) {
			g_warning("error with select(): %s", strerror(errno));
		}
		else if (ready >= 1) {

		  if (FD_ISSET(uwb_fd, &reader)) {
			  bytes = uwb_read(uwb_fd, buffer);

			  if (bytes < 0)  {
				  g_message("uwb_read() returned: %d.", bytes);
				  continue;
			  }
		  } 
		  if (FD_ISSET(hab_fd, &reader)) {
			hab_read();
		  }
		}
		else {
            node_remove();
		}
	}

	return 0;
}

