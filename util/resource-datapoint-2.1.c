
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <glib.h>

#include "bionet-util-2.1.h"
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

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
