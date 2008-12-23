
//
// Copyright (C) 2008, Regents of the University of Colorado.
//

#include "bionet-util.h"
#include "errno.h"

bionet_resource_t * bionet_value_get_resource(bionet_value_t * value)
{
    if (NULL == value)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_value_get_resource(): NULL value passed in");
	errno = EINVAL;
	return NULL;
    }

    return value->resource;
} /* bionet_value_get_resource() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
