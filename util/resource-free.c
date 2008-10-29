
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdlib.h>

#include <glib.h>

#include "bionet-util.h"


void bionet_resource_free(bionet_resource_t *resource) {
    // int i;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_free(): NULL Resource passed in");
        return;
    }

    if (resource->user_data != NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_free() called with Resource %s, user_data is not NULL, leaking memory now", resource->id);
    }

    // free all the datapoints
    // FIXME
#if 0
    for (i = 0; i < resource->datapoints.len; i ++) {
        bionet_datapoint_t *d = g_ptr_array_index(resource->datapoints, i);
        bionet_datapoint_free(d);
    }
    g_ptr_array_free(resource->datapoints, TRUE);
#endif

    free(resource);
}

