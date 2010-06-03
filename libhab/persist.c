
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#define _GNU_SOURCE
#include <string.h>

#include "hardware-abstractor.h"
#include "libhab-internal.h"
#include "protected.h"

int hab_persist_resource(bionet_resource_t * resource) {
    if (NULL == resource) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "hab_persist_resource: NULL resource passed in.");
	return 1;
    }

    if (bionet_resource_persist(resource, persist_dir)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "hab_persist_resource: Failed to make resource persistant");
	return 1;
    }

    return 0;
} /* hab_persist_resource() */


int hab_set_persist_directory(char * dir) {
    persist_dir = strdup(dir);
    if (NULL == persist_dir) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "hab_set_persist_directory: Failed to allocate memory for persistence dir - %m");
	return 1;
    }

    return 0;
} /* hab_set_persist_directory() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
