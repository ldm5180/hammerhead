
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "bionet-util.h"




bionet_datapoint_t *bionet_datapoint_new(
    bionet_resource_t *resource,
    const char *value_str,
    const struct timeval *timestamp
) {
    bionet_datapoint_t *d;
    int r;


    // sanity checking
    if (value_str == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_new(): NULL value passed in");
        return NULL;
    }


    if (
        (resource->data_type < BIONET_RESOURCE_DATA_TYPE_MIN) ||
        (resource->data_type > BIONET_RESOURCE_DATA_TYPE_MAX)
    ) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_new(): invalid data type %d", resource->data_type);
        return NULL;
    }


    d = (bionet_datapoint_t *)calloc(1, sizeof(bionet_datapoint_t));
    if (d == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "out of memory!");
        return NULL;
    }

    d->resource = resource;

    r = bionet_datapoint_value_from_string(d, value_str);
    if (r < 0) {
        // the value_from_string function will have logged an error message already, so we dont have to
        free(d);
        return NULL;
    }

    bionet_datapoint_set_timestamp(d, timestamp);

    return d;
}

