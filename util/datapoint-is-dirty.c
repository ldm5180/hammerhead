
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>

#include "internal.h"
#include "bionet-util.h"


int bionet_datapoint_is_dirty(const bionet_datapoint_t *datapoint) {
    if (NULL == datapoint)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_datapoint_is_dirty(): NULL datapoint passed in");
	errno = EINVAL;
	return -1;
    }

    return datapoint->dirty;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
