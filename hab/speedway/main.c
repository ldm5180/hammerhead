
// Copyright (c) 2008-2009, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "speedway.h"

/*
 * This is the speedway hab used for the habitat portals. It's the greatest 
 * hab ever written for man-kind.
 * 
 * 1) Connect to the reader.
 * 2) Verify that the connection is valid.
 * 3) Clear (scrub) the reader configuration.
 * 4) Add and enable the ROSpec.
 * 		- Uses all antennas.
 * 5) Run the ROSpec.
 *
 */

llrp_u16_t rf_sensitivity = 0;


int main(int argc, char *argv[]) {
    int i = 0;
    int bionet_fd;

    char* hab_type = "speedway";
    char* hab_id = NULL;
    char* reader_ip = NULL;

    for (i = 1; i < argc; i ++) {
        if (strcmp(argv[i], "--id") == 0) {
            i ++;
            hab_id = argv[i];

        } else if (strcmp(argv[i], "--gpi-delay") == 0) {
            i ++;
            gpi_delay = atoi(argv[i]);

        } else if (strcmp(argv[i], "--gpi-polarity") == 0) {
            i ++;
            gpi_polarity = atoi(argv[i]);

        } else if (strcmp(argv[i], "--num-scans") == 0) {
            i ++;
            num_scans = atoi(argv[i]);
	    
	} else if (strcmp(argv[i], "--rfsensitivity") == 0) {
	    i ++;
	    rf_sensitivity = atoi(argv[i]);

        } else if (strcmp(argv[i], "--scan-idle") == 0) {
            i ++;
            scan_idle = atoi(argv[i]);

        } else if (strcmp(argv[i], "--scan-timeout") == 0) {
            i ++;
            scan_timeout = atoi(argv[i]);

        } else if (strcmp(argv[i], "--show-messages") == 0) {
            show_messages = 1;

        } else if (strcmp(argv[i], "--help") == 0) {
            usage();
            exit(0);

        } else {
            if (i != (argc-1)) {
                g_warning("unknown command-line argument '%s'", argv[i]);
                exit(1);
            }
            reader_ip = argv[i];
        }
    }

    if(argc == 1) {
	usage();
	exit(0);
    }

    if (reader_ip == NULL) {
        g_warning("no reader IP specified (use --target)");
        exit(1);
    }


    // 
    // init bionet
    //

    hab = bionet_hab_new(hab_type, hab_id);

    bionet_fd = hab_connect(hab);
    if (bionet_fd < 0) {
        g_warning("could not connect to Bionet, exiting");
        return 1;
    }


    // 
    // init the Speedway reader
    //

    if (speedway_connect(reader_ip) != 0) {
        g_warning("speedway connect failed");
        goto end;
    }

    if (speedway_configure() != 0) {
        g_warning("speedway configure failed");
        goto end;
    }


    //
    // For the JSC Wireless Habitat demo, we want a GPI trigger.
    // The Speedway GPIs are not biased, and there is no +5 supply
    // available on the GPIO connector, so we use a GPO to provide the
    // voltage for the active-high activation signal.
    //
    // The circuit looks like this:
    //
    // GPO 1 ----
    //          |
    //          /  (a normally open momentary switch)
    //         /   
    //          |
    // GPI 1 ----
    //          |
    //          R  (33K resistor)
    //          |
    // GND   ----
    // 

    if (set_gpo(1, 1) != 0) {
        g_warning("speedway GPO setup failed");
        goto end;
    }


    get_reader_config();


    make_reader_node();


    //
    // here we're connected to the Speedway reader and it's all set up
    //
    // set up the main loop
    //


    main_loop = g_main_loop_new(NULL, TRUE);

    {
        GIOChannel *ch;
        ch = g_io_channel_unix_new(bionet_fd);
        g_io_add_watch(ch, G_IO_IN, read_from_bionet, NULL);
    }

    g_timeout_add(100, poll_reader, NULL);
        

    g_main_loop_run(main_loop);


    scrubConfiguration();

end: 
    LLRP_TypeRegistry_destruct(pTypeRegistry);

    exit(0);
}

