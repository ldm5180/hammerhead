
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "bionet-util.h"
#include "internal.h"


int bionet_resource_set_epsilon(bionet_resource_t * resource,
				      bionet_epsilon_t * epsilon) {
    if (NULL == resource) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_set_epsilon_epsilon: NULL resource passed in.");
	return 1;
    }

    if (resource->epsilon) {
	bionet_epsilon_free(resource->epsilon);
    }

    resource->epsilon = epsilon;

    return 0;
} /* bionet_resource_set_epsilon() */


int bionet_resource_set_delta(bionet_resource_t * resource,
			      struct timeval delta) {
    if (NULL == resource) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_set_epsilon_epsilon: NULL resource passed in.");
	return 1;
    }

    resource->delta.tv_sec  = delta.tv_sec; 
    resource->delta.tv_usec = delta.tv_usec; 

    return 0;
} /* bionet_resource_set_delta() */

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
