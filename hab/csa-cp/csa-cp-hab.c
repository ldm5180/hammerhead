
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdlib.h>
#include <string.h>

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

    {
        int i;

        for (i = 1; i < argc; i++) {
            if ((strcmp("-i", argv[i]) == 0) || (strcmp("--id", argv[i]) == 0)) {
                i++;
                if (argv[i] == NULL) {
                    print_help();
                    return 1;
                }
                hab_id = argv[i];
            } else if ((strcmp("-d", argv[i]) == 0) || (strcmp("--device-file", argv[i]) == 0)) {
                i++;
                if (argv[i] == NULL) {
                    print_help();
                    return 1;
                }
                device_file = argv[i];
            } else if (strcmp("--no-files", argv[i]) == 0) {
                record_raw_data = 0;
            } else if ((strcmp("-c", argv[i]) == 0) || (strcmp("--clear-files", argv[i]) == 0)) {
                clear_files = 0;
            } else if (strcmp("--debug", argv[i]) == 0) {
                log_context.log_limit = G_LOG_LEVEL_DEBUG;
            } else if ((strcmp("-?", argv[i]) == 0) || (strcmp("--help", argv[i]) == 0)) {
                print_help();
                return 0;
            } else {
                print_help();
                return 1;
            }
        }
    }

    if (device_file == NULL) {
        printf("please give valid file device file, exiting\n");
        return 1;
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

