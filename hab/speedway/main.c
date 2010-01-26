
// Copyright (c) 2008-2009, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>

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
    int i;
    int bionet_fd;

    char* hab_type = "speedway";
    char* hab_id = NULL;
    char* reader_ip = NULL;
    char* security_dir = NULL;

    signal(SIGINT, handle_interrupt);

    int c;
    while(1) {
	static struct option long_options[] = {
	    {"help", 0, 0, '?'},
	    {"version", 0, 0, 'v'},
	    {"id", 0, 0, 'i'},
	    {"gpi-delay", 1, 0, 'd'},
	    {"gpi-polarity", 1, 0, 'p'},
	    {"num-scans", 1, 0, 'n'},
	    {"rfsensitivity", 1, 0, 'r'},
	    {"scan-idle", 1, 0, 'l'},
	    {"scan-timeout", 1, 0, 't'},
	    {"show-messages", 0, 0, 'm'},
	    {"security-dir", 1, 0, 's'},
	    {"immediate-trigger", 0, 0, 'c'},
	    {"null-trigger", 0, 0, 'u'},
	    {"gpi-trigger", 0, 0, 'g'},
	    {0, 0, 0, 0} //this must be last in the list
	};

	c = getopt_long(argc, argv, "?vhd:p:n:r:i:t:ms:cug", long_options, &i);
	if (c == -1) {
	    break;
	}

	switch (c) {

	case '?':
	case 'h':
	    usage();
	    exit(0);

	case 'd':
	    gpi_delay = strtol(optarg, NULL, 0);
	    if (INT_MAX == gpi_delay) {
		g_log("", G_LOG_LEVEL_WARNING, "Failed to parse GPI Delay: %m");
	    }
	    break;

	case 'i':
	    hab_id = optarg;
	    break;

	case 'l':
	    scan_idle = strtol(optarg, NULL, 0);
	    if (INT_MAX == scan_idle) {
		g_log("", G_LOG_LEVEL_WARNING, "Failed to parse Scan Idle: %m");
	    }
	    break;

	case 'm':
	    show_messages = 1;
	    break;

	case 'n':
	    num_scans = strtol(optarg, NULL, 0);
	    if (INT_MAX == num_scans) {
		g_log("", G_LOG_LEVEL_WARNING, "Failed to parse Num Scans: %m");
	    }
	    break;

	case 'p':
	    gpi_polarity = strtol(optarg, NULL, 0);
	    if (INT_MAX == gpi_polarity) {
		g_log("", G_LOG_LEVEL_WARNING, "Failed to parse GPI Polarity: %m");
	    }
	    break;

	case 'r':
            {
                long tmp;
                errno = 0;
                tmp = strtol(optarg, NULL, 0);
                if (errno != 0) {
                    g_log("", G_LOG_LEVEL_WARNING, "Failed to parse RF Sensitivity: %m");
                    exit(1);
                }
                if (tmp > UINT16_MAX) {
                    g_log("", G_LOG_LEVEL_WARNING, "RF Sensitivity %ld is out of range, clipping to %d", tmp, (int)UINT16_MAX);
                    tmp = UINT16_MAX;
                }
                rf_sensitivity = tmp;
            }
            break;

	case 's':
	    security_dir = optarg;
	    break;

	case 't':
	    scan_timeout = strtol(optarg, NULL, 0);
	    if (INT_MAX == scan_timeout) {
		g_log("", G_LOG_LEVEL_WARNING, "Failed to parse Scan Timeout: %m");
	    }
	    break;

	case 'v':
	    print_bionet_version(stdout);
	    exit(0);

	case 'c':
	    immediate_trigger = 1;
	    break;

	case 'u':
	    immediate_trigger = 0;
	    null_trigger = 1;
	    break;

	case 'g':
	    immediate_trigger = 0;
	    gpi_trigger = 1;
	    break;

	default:
	    break;
	}
    }

    if(argc == 1) {
	usage();
	exit(0);
    }

    if (argv[argc-1][0] == '-') {
	g_warning("unknown command-line argument '%s'", argv[optind]);
	exit(1);
    } else {
	reader_ip = argv[optind];
    }

    if (reader_ip == NULL) {
        g_warning("no reader IP specified");
	usage();
        exit(1);
    }


    if (security_dir) {
        if (hab_init_security(security_dir, 1)) {
            g_log("", G_LOG_LEVEL_WARNING, "Failed to initialize security.");
        }
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

