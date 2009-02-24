
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>
#include <errno.h>

#include <glib.h>

#include "bionet-util.h"
#include "internal.h"


void bionet_datapoint_free(bionet_datapoint_t *d) {
    if (NULL == d)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_datapoint_free(): NULL Datapoint passed in");
	errno = EINVAL;
	return;
    }

    if (NULL != d->value) { 
	bionet_value_free(d->value);
    }
    free(d);
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
