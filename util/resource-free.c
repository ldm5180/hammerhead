
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
    while (bionet_resource_get_num_datapoints(resource) > 0) {
        bionet_datapoint_t *d = bionet_resource_get_datapoint_by_index(resource, 0);
        bionet_datapoint_free(d);
    }
    g_ptr_array_free(resource->datapoints, TRUE);

    free(resource);
}


void bionet_datapoint_free(bionet_datapoint_t *d) {
    if (d->resource->data_type == BIONET_RESOURCE_DATA_TYPE_STRING) {
        if (d->value.string_v != NULL) {
            free(d->value.string_v);
        }
    }
}

