
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#ifdef BIONET_21_API
#include "internal.h"
#include "bionet-util-2.1.h"
#else
#include "bionet-util.h"
#endif

#include <errno.h>


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
