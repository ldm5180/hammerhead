
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <stdlib.h>

#include "internal.h"
#include "bionet-util.h"


int bionet_bdm_is_secure(const bionet_bdm_t *bdm) {
    if (NULL == bdm) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_bdm_is_secure(): NULL bdm passed in.");
	return -1;
    }
    return bdm->is_secure;
} /* bionet_bdm_is_secure() */


void bionet_bdm_set_secure(bionet_bdm_t *bdm, int is_secure) {
    if (NULL == bdm) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_bdm_set_secure(): NULL bdm passed in.");
	return;
    }

    bdm->is_secure = is_secure;

    return;
} /* bionet_bdm_set_secure() */
