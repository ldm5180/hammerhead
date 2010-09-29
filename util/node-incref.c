
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>

#include <glib.h>

#include "internal.h"
#include "bionet-util.h"


void bionet_node_increment_ref_count(bionet_node_t * node) {
    if (NULL == node) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_increment_ref_count: NULL NODE passed in.");
	return;
    }

    node->ref = node->ref + 1;

    return;
} /* bionet_node_increment_ref_count() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
