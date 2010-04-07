
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <stdlib.h>

#include "internal.h"
#include "bionet-util.h"


int bionet_hab_is_secure(const bionet_hab_t *hab) {
    if (NULL == hab) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_hab_is_secure(): NULL hab passed in.");
	return 0;
    }
    return hab->is_secure;
} /* bionet_hab_is_secure() */


void bionet_hab_set_secure(bionet_hab_t *hab, int is_secure) {
    if (NULL == hab) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_hab_set_secure(): NULL hab passed in.");
	return;
    }

    hab->is_secure = is_secure;

    return;
} /* bionet_hab_set_secure() */
