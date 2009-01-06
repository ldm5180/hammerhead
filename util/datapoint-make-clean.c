
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>

#include "internal.h"
#include "bionet-util.h"


void bionet_datapoint_make_clean(bionet_datapoint_t *datapoint) {
    if (NULL == datapoint)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_datapoint_make_clean(): NULL datapoint passed in");
	errno = EINVAL;
	return;
    }

    datapoint->dirty = 0;
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
