
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


void bionet_epsilon_free(bionet_epsilon_t *epsilon)
{
    /* sanity */
    if (NULL == epsilon)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
	      "bionet_epsilon_free(): NULL epsilon passed in");
	errno = EINVAL;
	return;
    }
    
    free(epsilon);

    return;
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
