
#include <errno.h>
#include <signal.h>
#include <stdio.h>
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
		g_warning("scrubConfiguration error");
	}

	r = addROSpec();

	if (r != 0) {
		g_warning("addROSpec error");
	}

	r = enableROSpec();

	if (r != 0) {
		g_warning("enableROSpec error");
	}

	return r;
}

