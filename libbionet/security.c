
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <stdio.h>

#include "bionet-util.h"

#include "libbionet-internal.h"
#include "bionet.h"
#include "cal-client.h"

#include "glib.h"

void * libbionet_ssl_ctx = NULL;
int libbionet_require_security = 0;

int bionet_init_security(const char * dir, int require) {
    if (libbionet_cal_handle != NULL) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "bionet_init_security(): CAL is already initialized.");
	return (-1);
    }

    libbionet_require_security = require;

    libbionet_ssl_ctx = cal_client.init_security(dir, require);
    if ((NULL == libbionet_ssl_ctx) && (require)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
	      "bionet_init_security(): Initializing CAL security failed.");
	return (-1);
    }

    //g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO,
    //	  "bionet_init_security(): CAL security initialized.");
    return 0;
} /* bionet_security_set_dir() */

