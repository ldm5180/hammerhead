
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
  	{ "pal-ip",      'a', 0, G_OPTION_ARG_STRING,  &pal_ip,      "IP address of the PAL-650",                                        NULL },
	{ "port",        'p', 0, G_OPTION_ARG_INT,     &pal_port,    "PAL-650 Data Port (defaults to 5117).",                            NULL },
	{ "tag-timeout", 't', 0, G_OPTION_ARG_INT,     &tag_timeout, "Seconds of RFID Tag absence before reporing the Bionet Node lost", NULL },
	{ NULL }
};


int main(int argc, char *argv[]) {
    int bionet_fd;
    int pal_fd;


    // this writes the log messages to the correct location
    bionet_log_context_t log_context = {
        // destination: BIONET_LOG_TO_SYSLOG,
        destination: BIONET_LOG_TO_STDOUT,
        log_limit: G_LOG_LEVEL_INFO
    };

    g_log_set_default_handler(bionet_glib_log_handler, &log_context);


    // Parse command-line arguments
    {
        int r;
        GError *error = NULL;
        GOptionContext *context;

        context = g_option_context_new ( "- A Bionet Hardware Abstractor for the PAL-650(tm)");

        g_option_context_add_main_entries (context, entries, NULL);

        r = g_option_context_parse (context, &argc, &argv, &error);

        if (! r) {
            g_warning("error parsing");
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


    // Connect to Bionet.
    hab = bionet_hab_new("PAL-650", NULL);
    if (hab == NULL) {
        g_warning("error creating HAB structure!");
        exit(1);
    }

    hab_register_callback_set_resource(cb_set_resource);

    bionet_fd = hab_connect(hab);
    if (bionet_fd < 0) {
        g_warning("error connecting to Bionet");
        exit(1);
    }

    pal_fd = -1;  // we'll initialize it in the main loop below

    make_shutdowns_clean();

    while (1) {
        int r;
        int max_fd;
        fd_set readers;
        struct timeval timeout;

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
        FD_SET(bionet_fd, &readers);
        FD_SET(pal_fd, &readers);

        max_fd = Max(bionet_fd, pal_fd) + 1;

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        r = select(max_fd, &readers, NULL, NULL, &timeout);

        if (r < 0) {
            g_warning("error with select: %s", strerror(errno));
            continue;
        }

#if 0
		/*
		 * No need to make a request to the PAL.
		 *
        if (r == 0) {
            // timeout
            pal_request_taglist(pal_fd);
        }
		*/
#endif

        timeout_tags();

        if (FD_ISSET(bionet_fd, &readers)) {
            hab_read();
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

