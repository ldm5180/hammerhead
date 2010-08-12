
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <glib.h>
#include "bionet-util.h"
#include "libhab-internal.h"

void libhab_datapoint_destructor(bionet_datapoint_t * dp, void * hash_table) {
    GHashTable * hash = (GHashTable *)hash_table;
    bionet_resource_t * resource;

    /* sanity checking */
    if (NULL == hash) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "libhab_datapoint_destructor: NULL hash table passed in.");
	return;
    }
    if (NULL == dp) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "libhab_datapoint_destructor: NULL resource passed in.");
	return;
    }

    resource = bionet_datapoint_get_resource(dp);
    if (NULL == resource) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "libhab_datapoint_destructor: datapoint has no resource.");
	return;
    }

    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	  "libhab_datapoint_destructor: Removing key %s from recent datapoint hash", 
	  bionet_resource_get_name(resource));
} /* libhab_datapoint_destructor() */

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
