#include <errno.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <glib.h>

#include "ltkc.h"
#include "speedway.h"

/*
 * The steps done here are:
 * 	- Clear (scrub) the reader configuration.
 * 	- Configure for what we want to do.
 */

int speedway_configure() {
	int r = 0;

	r = scrubConfiguration();	

	if (r != 0) {
		printf("Error: scrubConfiguration error\n");
	}

	r = addROSpec();

	if (r != 0) {
		printf("Error: addROSpec error\n");
	}

	r = enableROSpec();

	if (r != 0) {
		printf("Error: enableROSpec error\n");
	}

	return r;
}

