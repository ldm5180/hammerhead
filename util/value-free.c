
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <errno.h>

#include "bionet-util.h"
#include "internal.h"


void bionet_value_free(bionet_value_t *value)
{
    /* sanity */
    if (NULL == value)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
	      "bionet_value_free(): NULL value passed in");
	errno = EINVAL;
	return;
    }
    
    if (value->ref) {
	value->ref = value->ref - 1;
	return;
    }

    /* free any additional memory */
    if ((BIONET_RESOURCE_DATA_TYPE_STRING == value->resource->data_type) 
	&& (value->content.string_v))
    {
	/* this value is string and the content has been set */
	free(value->content.string_v);
    }

    free(value->as_string);

    free(value);

    return;
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
