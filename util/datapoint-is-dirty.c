
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>

#ifdef BIONET_21_API
#include "internal.h"
#include "bionet-util-2.1.h"
#else
#include "bionet-util.h"
#endif

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
