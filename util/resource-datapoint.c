
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <glib.h>

#include "bionet-util.h"
#include "internal.h"

void bionet_resource_add_datapoint(bionet_resource_t *resource,
				   bionet_datapoint_t *new_datapoint) 
{
    if (NULL == resource)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_add_datapoint(): NULL Resource passed in");
	errno = EINVAL;
	return;
    }

    if (NULL == new_datapoint)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_add_datapoint(): NULL Datapoint passed in");
	errno = EINVAL;
	return;
    }

    g_ptr_array_add(resource->datapoints, new_datapoint);
} 


void bionet_resource_remove_datapoint_by_index(
    bionet_resource_t *resource,
    unsigned int index
) {
    bionet_datapoint_t *d;

    if (resource == NULL) {
        g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING, 
            "bionet_resource_remove_datapoint_by_index(): NULL Resource passed in"
        );
	return;
    }

    if (index >= resource->datapoints->len) return;

    d = g_ptr_array_remove_index(resource->datapoints, index);

    bionet_datapoint_free(d);
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
