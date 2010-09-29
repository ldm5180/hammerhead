
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>

#include <glib.h>

#include "internal.h"
#include "bionet-util.h"


void bionet_hab_increment_ref_count(bionet_hab_t * hab) {
    if (NULL == hab) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_increment_ref_count: NULL HAB passed in.");
	return;
    }

    hab->ref = hab->ref + 1;

    return;
} /* bionet_hab_increment_ref_count() */


unsigned int bionet_hab_get_ref_count(bionet_hab_t * hab) {
    if (NULL == hab) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_get_ref_count: NULL HAB passed in.");
	return 0;
    }

    return hab->ref;
} /* bionet_hab_get_ref_count() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
