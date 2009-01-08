
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdlib.h>

#include <glib.h>

#include "bionet-util.h"
#include "internal.h"

void bionet_resource_free(bionet_resource_t *resource) {
    // int i;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_free(): NULL Resource passed in");
        return;
    }

    if (resource->user_data != NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_free() called with Resource %s, user_data is not NULL, leaking memory now", resource->id);
    }

    if (resource->id != NULL) {
        free(resource->id);
    }

    if (resource->name != NULL) {
        free(resource->name);
    }

    // free all the datapoints
    while(resource->datapoints->len > 0) {
        bionet_datapoint_t *d = g_ptr_array_remove_index_fast(resource->datapoints, 0);
        bionet_datapoint_free(d);
    }
    g_ptr_array_free(resource->datapoints, TRUE);

    free(resource);
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
