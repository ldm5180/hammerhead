
// Copyright (c) 2008-2010, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"


int speedway_configure() {
    int r;

    if (scrub_config) {
	r = scrubConfiguration();	
	if (r != 0) {
	    g_warning("scrubConfiguration error");
	    return -1;
	}
    }

    r = get_reader_capabilities();
    if (r != 0) {
	g_warning("Error getting reader capabilities");
	return -1;
    }

    r = configure_reader();
    if (r != 0) {
        // an error has been logged
        return -1;
    }

    if (scrub_config) {
	if (immediate_trigger == 1) {
	    r = addROSpec_Immediate(simple_report);
	} else if (null_trigger == 1) {
	    r = addROSpec_Null(simple_report);
	} else if (gpi_trigger == 1) {
	    r = addROSpec_GPI(simple_report);
	}
	if (r != 0) {
	    g_warning("addROSpec error");
	    return -1;
	}

	r = enableROSpec(123);
	if (r != 0) {
	    g_warning("enableROSpec error");
	    return -1;
	}
    }

    return 0;
}

