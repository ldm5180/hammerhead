
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <glib.h>

#include "hardware-abstractor.h"
#include "streamy-hab.h"




bionet_hab_t *this_hab = NULL;

GMainLoop *main_loop = NULL;

double bandwidth_limit = -1.0;

void usage(FILE *fp) {
    fprintf(fp,
	    "'streamy-hab' Hardware Abstractor for streaming data.\n"
	    "\n"
	    "usage: streamy-hab [OPTIONS]\n"
	    " -?,-h,--help                 Print this help\n"
	    " -i,--id <ID>                 Set the HAB-ID (hostname)\n"
	    " -b,--bandwidth-limit <float> Desired max send bandwidth per stream (KB/s)\n"
	    " -s,--security-dir <dir>      Directory containing security certificates\n"
	    " -v,--version                 Print the version number\n");

    return;
} /* usage() */

int main(int argc, char *argv[]) {
    int bionet_fd;
    char *id = NULL;
    char * security_dir = NULL;

    g_log_set_default_handler(bionet_glib_log_handler, NULL);


    // 
    // handle command-line arguments
    //
    int c;
    int i;
    while(1) {
	static struct option long_options[] = {
	    {"help", 0, 0, '?'},
	    {"version", 0, 0, 'v'},
	    {"bandwidth-limit", 1, 0, 'b'},
	    {"id", 1, 0, 'i'},
	    {"security-dir", 1, 0, 's'},
	    {0, 0, 0, 0} //this must be last in the list
	};

	c = getopt_long(argc, argv, "?b:hvi:s:", long_options, &i);
	if (c == -1) {
	    break;
	}

	switch (c) {

	case '?':
	case 'h':
	    usage(stdout);
	    exit(0);

	case 'b':
	    bandwidth_limit = strtoul(optarg, NULL, 10);
	    break;

	case 'i':
	    id = optarg;
	    break;

	case 's':
	    security_dir = optarg;
	    break;

	case 'v':
	    print_bionet_version(stdout);
	    exit(0);

	default:
	    break;
	}
    }

    if (security_dir) {
        if (hab_init_security(security_dir, 1)) {
            g_log("", G_LOG_LEVEL_WARNING, "Failed to initialize security.");
        }
    }

    //
    //  Initialize the HAB & connect to Bionet
    //

    hab_register_callback_stream_subscription(cb_stream_subscription);
    hab_register_callback_stream_unsubscription(cb_stream_unsubscription);
    hab_register_callback_stream_data(cb_stream_data);

    this_hab = bionet_hab_new("Streamy", id);

    bionet_fd = hab_connect(this_hab);
    if (bionet_fd < 0) {
        fprintf(stderr, "problem connecting to Bionet, exiting\n");
        return 1;
    }


    //
    //  Make the Nodes to hold the Streams
    //

    make_incoming_node(this_hab);
    make_outgoing_node(this_hab);


    //  
    // set up and start the main loop
    //

    main_loop = g_main_loop_new(NULL, TRUE);

    {
        GIOChannel *ch;
        ch = g_io_channel_unix_new(bionet_fd);
        g_io_add_watch(ch, G_IO_IN, read_from_bionet, NULL);
    }


    g_main_loop_run(main_loop);


    exit(0);
}

