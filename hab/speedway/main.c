
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
	int local_dbg = 1;

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

            } else {
                fprintf(stderr, "unknown command-line argument '%s'\n", argv[i]);
                exit(1);
            }
        }

        if (reader_ip == NULL) {
            fprintf(stderr, "no reader IP specified (use --target)\n");
            exit(1);
        }

	if (local_dbg) {
		printf("reader_ip = %s\n", reader_ip);
		printf("hab_type = %s\n", hab_type);
		printf("hab_id = %s\n", hab_id);
	}


        // 
        // init bionet
        //

	hab = bionet_hab_new(hab_type, hab_id);

	bionet_fd = hab_connect(hab);
	if (bionet_fd < 0) {
		printf("Error: could not connect, exiting\n");
		return 1;
	}


        // 
        // init the Speedway reader
        //

	if (speedway_connect(reader_ip) != 0) {
		printf("Error: speedway connect failed\n");
		goto end;
	}

	if (speedway_configure() != 0) {
		printf("Error: speedway configure failed\n");
		goto end;
	}


    //
    // here we're connected to the Speedway reader and it's all set up
    //


    do {
        struct timeval timeout;
        fd_set readers;
        int r;

        printf("INFO: Starting run\n");

        poll_for_report();

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
            g_message("*** bionet needs attention ***");
            hab_read();
        }
    } while(1);


    scrubConfiguration();

end: 
    LLRP_TypeRegistry_destruct(pTypeRegistry);

    exit(0);
}

