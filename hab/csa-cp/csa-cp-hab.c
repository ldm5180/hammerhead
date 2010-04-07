
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <glib.h>

#include "hardware-abstractor.h"
#include "csa-cp-hab.h"
#include "serial.h"


#define HAB_TYPE                     "CSA-CP"

bionet_hab_t *this_hab = NULL;


int main (int argc, char* argv[]) {
    char *device_file = NULL;
    char *hab_id = NULL;
    int bionet_fd;

    int clear_files = 1;       // If set to zero deletes the files on the monitor
    int record_raw_data = 1;   // if true, writes all data from the CSA-CP to a file

    serial_handle_t serial_handle;

    char * security_dir = NULL;

    //
    //  This connects writes the error messages to the correct location
    //

    bionet_log_context_t log_context = {
        destination: BIONET_LOG_TO_STDOUT,
        log_limit: G_LOG_LEVEL_INFO
    };
    g_log_set_default_handler(bionet_glib_log_handler, &log_context);


    //
    // parse command-line arguments
    //
    int i;
    int c;
    while (1) {
	static struct option long_options[] = {
	    {"help", 0, 0, '?'},
	    {"version", 0, 0, 'v'},
	    {"clear-files", 0, 0, 'c'},
	    {"debug", 0, 0, 'b'},
	    {"device-file", 1, 0, 'd'},
	    {"id", 1, 0, 'i'},
	    {"no-files", 0, 0, 'n'},
	    {"security-dir", 1, 0, 's'},
	    {0, 0, 0, 0} //this must be last in the list
	};

	c = getopt_long(argc, argv, "?hvcbnd:i:s:", long_options, &i);
	if (c == -1) {
	    break;
	}

	switch (c) {
	    
	case '?':
	case 'h':
	    print_help();
	    return 0;

	case 'b':
	    log_context.log_limit = G_LOG_LEVEL_DEBUG;
	    break;

	case 'c':
	    clear_files = 0;
	    break;

	case 'd':
	    device_file = optarg;
	    break;

	case 'i':
	    hab_id = optarg;
	    break;

	case 'n':
	    record_raw_data = 0;
	    break;

	case 's':
	    security_dir = optarg;
	    break;

	case 'v':
	    print_bionet_version(stdout);
	    return 0;

	default:
	    break;
	}
    }

    if (device_file == NULL) {
        printf("please give valid file device file, exiting\n");
        return 1;
    }

    if (security_dir) {
        if (hab_init_security(security_dir, 1)) {
            g_log("", G_LOG_LEVEL_WARNING, "Failed to initialize security.");
        }
    }

    //
    //  This sets up the HAB...
    //

    this_hab = bionet_hab_new(HAB_TYPE, hab_id);

    bionet_fd = hab_connect(this_hab);
    if (bionet_fd < 0) {
        g_log("", G_LOG_LEVEL_ERROR, "error connecting to Bionet");
        exit(1);
    }


    make_shutdowns_clean();


    //
    //  and the main while loop
    //
    //  NOTE: this HAB does not get any input at all from Bionet - it just publishes data out
    //

    while (1) {
        int i;
        int num_files;


        // probe for the CSA-CP 
        g_debug("waiting for CSA-CP");
        while (1) {
            serial_handle = csa_cp_probe(device_file);
            if (serial_handle != INVALID_HANDLE_VALUE) break;
            g_usleep(1000 * 1000);
        }


        // 
        // when we get here there's an established connection to the CSA-CP
        //


        // now we get the number of sessions stored on the CSA-CP...
        num_files = get_num_files(serial_handle);
        if (num_files < 0) {
            serial_close(serial_handle);
            g_log("", G_LOG_LEVEL_DEBUG, "could not find number of open files\n");
            continue;
        }
        g_log("", G_LOG_LEVEL_DEBUG, "files available: %d\n", num_files);


        //
        // download all files
        //

        for (i = 0; i < num_files; i++) {
            g_debug("downloading file %d/%d", (i+1), num_files);

            if (download_session(serial_handle, i, record_raw_data) < 0) {
                g_log("", G_LOG_LEVEL_WARNING, "download failed");
            }

            if (clear_files == 0) {
                if (clear_csa(serial_handle) != 0) {
                    g_log("", G_LOG_LEVEL_WARNING, "could not clear csa-cp\n");
                }
            }
        }


        // 
        // When we get here, we've downloaded all the files from the CSA-CP
        // and we're done with it.  Wait for it to go away.
        //

        g_debug("waiting for CSA-CP to go away");
        while (csa_cp_ping(serial_handle)) {
            g_usleep(1000*1000);
        }


        //
        // ok, now it's been powered off or disconnected
        //

        serial_close(serial_handle);
    }

    return 0;
}

