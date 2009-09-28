
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <stdio.h>

#include "bionet-util.h"

#include "libhab-internal.h"
#include "hardware-abstractor.h"
#include "cal-server.h"

#include "glib.h"


int hab_init_security(const char * dir, int require) {
    if (libhab_cal_fd != -1) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "hab_init_security(): CAL is already initialized.");
	return (-1);
    }

    if (cal_server.init_security(dir, require)) {
	return 0;
    }

    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
	  "hab_init_security(): Initializing CAL security failed.");
    return (-1);
} /* bionet_security_set_dir() */

