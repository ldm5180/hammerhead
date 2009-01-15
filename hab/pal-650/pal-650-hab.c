
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/select.h>

#include <glib.h>

#include <hardware-abstractor.h>

#include "pal-650-hab.h"


static GOptionEntry entries[] = {
	{"nag", 'n', 0, G_OPTION_ARG_STRING, &nag_hostname,
  		"NAG hostname", NULL},
  	{"pal-ip", 'a', 0, G_OPTION_ARG_STRING,  &pal_ip, 
  		"IP address of the PAL-650", NULL}, 
	{"port", 'p', 0, G_OPTION_ARG_INT, &pal_port,
		"PAL-650 Data Port", NULL},
	{"tag-timeout", 't', 0, G_OPTION_ARG_INT, &tag_timeout, 
  		"Seconds of RFID Tag absence before reporing the Bionet Node lost", 
		NULL},
	{NULL}
};


int main(int argc, char *argv[]) {
    int nag_fd;
    int pal_fd;


	/*
	 * This writes the error messages to the correct location.
	 */
    bionet_log_context_t log_context = {
        // destination: BIONET_LOG_TO_SYSLOG,
        destination: BIONET_LOG_TO_STDOUT,
        log_limit: G_LOG_LEVEL_INFO
    };

    g_log_set_default_handler(bionet_glib_log_handler, &log_context);


	/*
	 * Parse command-line arguments
	 */
    {
        int r;
        GError *error = NULL;
        GOptionContext *context;

        context = g_option_context_new (
			"- A Bionet Hardware Abstractor for the PAL-650(tm)");

        g_option_context_add_main_entries (context, entries, NULL);

        r = g_option_context_parse (context, &argc, &argv, &error);

        if (! r) {
            g_warning("error parsing");
            exit(1);
        }

        if (nag_hostname == NULL) {
            g_warning("No NAG hostname specified!");
            exit(1);
        }

        if (pal_ip == NULL) {
            g_warning("PAL-650 IP not specified!");
            exit(1);
        }

		if (pal_port == 0) {
			g_warning("PAL-650 Port not specified!");
			exit(1);
		}
    }

	/*
	 * Initialize the nodes hash.
	 */
    nodes = g_hash_table_new_full(g_str_hash, g_str_equal, free, drop_node);

	/*
	 * Connect to Bionet.
	 */
    hab_set_type("PAL-650");
    hab_set_nag_hostname(nag_hostname);
    hab_register_callback_set_resource(cb_set_resource);

    nag_fd = -1;  // we'll initialize it in the main loop below
    pal_fd = -1;  // we'll initialize it in the main loop below

    make_shutdowns_clean();

	int r;
   	int max_fd;
   	fd_set readers;
   	struct timeval timeout;

    while (1) {
		// Connect to the NAG.
   		if (nag_fd < 0) {
   			nag_fd = hab_connect_to_nag();

       		if (nag_fd < 0) {
       	 		g_warning("Error connecting to NAG");
       	    	sleep(3);

       	    	continue;
			}

       		g_message("Connected to the NAG");
		}

		// Connect to the PAL-650.
        if (pal_fd < 0) {
            pal_fd = pal_connect(pal_ip, pal_port);

            if (pal_fd < 0) {
                g_warning("Error connecting to PAL-650");
                sleep(5);

                continue;
            }

            g_message("Connected to the PAL-650");
        }

        // g_log("", G_LOG_LEVEL_INFO, "top of main loop");

        FD_ZERO(&readers);
        FD_SET(nag_fd, &readers);
        FD_SET(pal_fd, &readers);

        max_fd = Max(nag_fd, pal_fd) + 1;

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        r = select(max_fd, &readers, NULL, NULL, &timeout);

        if (r < 0) {
            g_warning("error with select: %s", strerror(errno));
            continue;
        }

		/*
		 * No need to make a request to the PAL.
		 *
        if (r == 0) {
            // timeout
            pal_request_taglist(pal_fd);
        }
		*/

        timeout_tags();

        if (FD_ISSET(nag_fd, &readers)) {
            hab_read_from_nag();
            hab_handle_queued_nag_messages();

            continue;
        }

        if (FD_ISSET(pal_fd, &readers)) {
            int r;

            r = pal_read(pal_fd);

            if (r < 0) {
                close(pal_fd);
                pal_fd = -1;
            }

            continue;
        }
    }

    exit(0);
}

