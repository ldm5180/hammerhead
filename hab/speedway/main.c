
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

int main(int argc, char *argv[]) {
    int i = 0;
    int bionet_fd;

    char* hab_type = "speedway";
    char* hab_id = NULL;
    char* reader_ip = NULL;


    for (i = 1; i < argc; i ++) {
        if (strcmp(argv[i], "--target") == 0) {
            i ++;
            reader_ip = argv[i];

        } else if (strcmp(argv[i], "--id") == 0) {
            i ++;
            hab_id = argv[i];

        } else if (strcmp(argv[i], "--show-messages") == 0) {
            show_messages = 1;

        } else {
            g_warning("unknown command-line argument '%s'", argv[i]);
            exit(1);
        }
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


    //
    // here we're connected to the Speedway reader and it's all set up
    //

    do {
        struct timeval timeout;
        fd_set readers;
        int r;

        FD_ZERO(&readers);
        FD_SET(bionet_fd, &readers);

        timeout.tv_sec = 0;
        timeout.tv_usec = 100 * 1000;

        r = select(bionet_fd + 1, &readers, NULL, NULL, &timeout);
        if (r < 0) {
            if (errno == EINTR) continue;
            g_warning("error with select(): %s", strerror(errno));
            exit(1);
        } else if (r == 1) {
            hab_read();
            continue;
        }

        // only if bionet didnt have anything to do, do we check the reader
        poll_for_report();

    } while(1);


    scrubConfiguration();

end: 
    LLRP_TypeRegistry_destruct(pTypeRegistry);

    exit(0);
}

