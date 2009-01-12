
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
	char* hab_id = "storm";
	char* reader_ip = "192.168.5.42";

	bionet_hab_t *hab;


        for (i = 1; i < argc; i ++) {
            if (strcmp(argv[i], "--target") == 0) {
                i ++;
                reader_ip = argv[i];

            } else {
                fprintf(stderr, "unknown command-line argument '%s'\n", argv[i]);
                exit(1);
            }
        }


	// todo: parse cmd line args.

	if (local_dbg) {
		printf("reader_ip = %s\n", reader_ip);
		printf("hab_type = %s\n", hab_type);
		printf("hab_id = %s\n", hab_id);
	}

	// Initialize hab with bionet naming, type.id.
	hab = bionet_hab_new(hab_type, hab_id);

	// Connect to bionet.
	bionet_fd = hab_connect(hab);
	
	if (bionet_fd < 0) {
		printf("Error: could not connect, exiting\n");

		return 1;
	}

	add_node(hab);

	if (speedway_connect(reader_ip) != 0) {
		printf("Error: speedway connect failed\n");
		goto end;
	}

	if (scrubConfiguration() != 0) {
		printf("Error: scrub config failed\n");
		goto end;
	}

	if (addROSpec() != 0) {
		printf("Error: addROSpec failed\n");
		goto end;
	}

	if (enableROSpec() != 0) {
		printf("Error: enableROSpec failed\n");
		goto end;
	}

	for (i = 1; i < 5; i++) {
		printf("INFO: Starting run %d \n", i);

		if (startROSpec() != 0) {
			printf("INFO: startROSpec failed, %d \n", i);

			break;
		}

		if (awaitAndPrintReport()) {
			break;
		}
	}

	scrubConfiguration();

	end: 
		LLRP_TypeRegistry_destruct(pTypeRegistry);

    exit(0);
}

