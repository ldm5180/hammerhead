/*
 * Written to interface with the Time Domain radio.
 *
 * G.L. Grobe
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <glib.h>

#include <hardware-abstractor.h>

#include "td-radio.h"

static GOptionEntry entries[] = {
	{"nag", 'n', 0, G_OPTION_ARG_STRING,&nag_hostname, "NAG hostname", NULL},
	{"ip", 'a', 0, G_OPTION_ARG_STRING, &ip, "IP address of radio", NULL},
	{"port", 'p', 0, G_OPTION_ARG_INT, &port, "Radio data port", NULL}, 
	{"timeout", 't', 0, G_OPTION_ARG_INT, &timeout, 
		"Seconds of absense before reporting no data.", NULL}, 
	{NULL}
};

int main(int argc, char *argv[]) {
	int nag_fd = -1;
	int radio_fd = -1;
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

	if (nag_hostname == NULL) {
		g_warning("No NAG hostname specified!");
		exit(1);
	}

	if (ip == NULL) {
		g_warning("Radio IP not specified.");
		exit(1);
	}

	if (port == 0) {
		g_warning("Radio port not specified.");
		exit(1);
	}

	hab_set_type("TD-P210");
	hab_set_nag_hostname(nag_hostname);
	hab_register_callback_set_resource(cb_set_resource);

	make_shutdowns_clean();

	buffer = calloc(BUFFER_SZ, sizeof(unsigned int));

	if (buffer == NULL) {
		g_warning("could not allocation receive buffer.");

		return -1;
	}

	fd_set reader;
	struct timeval tv;

	while (1) {

		// If we don't have our connection to the NAG, then connect.
		if (nag_fd < 0) {
			nag_fd = hab_connect_to_nag();

			if (nag_fd < 0) {
				g_warning("Error connecting to the NAG");
				sleep(3);

				continue;
			}

			g_message("Connected to the NAG.");
		}

		// Likewise, make sure we're connected to the radio.
		if (radio_fd < 0) {
			radio_fd = radio_connect(ip, port);

			if (radio_fd < 0) {
				g_warning("Connection failed.");
				return -1;
			}

			if (radio_send(radio_fd) < 0) {
				g_warning("Could not send message.");
			}

			g_message("Connected to the TD-P210.");
			g_message("Radio IP address: %s", ip);
			g_message("Radio port: %d", port);
			g_message("Timeout: %d", timeout);
		}

		FD_ZERO(&reader);
		FD_SET(radio_fd, &reader);

		tv.tv_sec = timeout;
		tv.tv_usec = 0;

		int ready = select(radio_fd + 1, &reader, NULL, NULL, &tv);

		if (ready == -1) {
			g_warning("error with select(): %s", strerror(errno));
		}
		else if (ready) {
			bytes = radio_read(radio_fd, buffer);

			if (bytes < 0)  {
				g_message("radio_read() returned: %d.", bytes);
				continue;
			}

			switch(process_data(buffer)) {
				case NO_SCAN: 
					g_message("NO_SCAN");
					// nothing to do.
					break;

				case WRONG_SEGMENT: 
					g_message("WRONG_SEGMENT");
					// nothing to do.
					break;

				case NEXT_SEGMENT:
					g_message("NEXT_SEGMENT");
					// nothing to do.
					break;

				case SCAN_COMPLETE:
					g_message("SCAN_COMPLETE");
					// todo: write to file.	
					//
					break;
			}
		}
		else {
			g_message("No data for the last %d seconds.", timeout);
		}
	}

	return 0;
}

void print_buffer() {
/*	g_message("scanNumber: %d", scan->scanNumber);
	g_message("segmentNumber: %d", scan->segmentNumber);
	g_message("numScanPointsTotal: %d", scan->numScanPointsTotal); 
	g_message("maxScanPointsSegment: %d", scan->maxScanPointsSegment); 
	g_message("numScanPointsSegment: %d", scan->numScanPointsSegment); 
	g_message("scanRateRx: %d", scan->scanRateRx); 
	g_message("scanRateTx: %d", scan->scanRateTx); */
}

