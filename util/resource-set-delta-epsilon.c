
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "bionet-util.h"
#include "internal.h"


int bionet_resource_set_delta_epsilon(bionet_resource_t * resource,
				      bionet_delta_t * delta,
				      struct timeval epsilon) {
    if (NULL == resource) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_set_delta_epsilon: NULL resource passed in.");
	return 1;
    }

    if (resource->delta) {
	bionet_delta_free(resource->delta);
    }

    resource->delta = delta;

    resource->epsilon.tv_sec  = epsilon.tv_sec; 
    resource->epsilon.tv_usec = epsilon.tv_usec; 

    return 0;
} /* bionet_resource_set_delta_epsilon_binary() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
