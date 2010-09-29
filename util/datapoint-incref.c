
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>

#include <glib.h>

#include "internal.h"
#include "bionet-util.h"


void bionet_datapoint_increment_ref_count(bionet_datapoint_t * datapoint) {
    if (NULL == datapoint) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_increment_ref_count: NULL DATAPOINT passed in.");
	return;
    }

    datapoint->ref = datapoint->ref + 1;

    return;
} /* bionet_datapoint_increment_ref_count() */


unsigned int bionet_datapoint_get_ref_count(bionet_datapoint_t * datapoint) {
    if (NULL == datapoint) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_get_ref_count: NULL DATAPOINT passed in.");
	return 0;
    }

    return datapoint->ref;
} /* bionet_datapoint_get_ref_count() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
