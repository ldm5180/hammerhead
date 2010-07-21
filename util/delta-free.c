
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


void bionet_delta_free(bionet_delta_t *delta)
{
    /* sanity */
    if (NULL == delta)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
	      "bionet_delta_free(): NULL delta passed in");
	errno = EINVAL;
	return;
    }
    
    free(delta);

    return;
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
