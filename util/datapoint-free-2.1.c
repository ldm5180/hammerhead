
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdlib.h>
#include <errno.h>

#include <glib.h>

#include "bionet-util-2.1.h"
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
