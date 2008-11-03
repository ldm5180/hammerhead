
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "bionet-util.h"




bionet_datapoint_t *bionet_resource_add_datapoint(
    bionet_resource_t *resource,
    const char *value_str,
    const struct timeval *timestamp
) {
    bionet_datapoint_t *d;

    d = bionet_datapoint_new_with_valuestr(resource, value_str, timestamp);
    if (d == NULL) return NULL;

    g_ptr_array_add(resource->datapoints, d);

    return d;
}


void bionet_resource_add_existing_datapoint(
    bionet_resource_t *resource,
    bionet_datapoint_t *new_datapoint
) {
    g_ptr_array_add(resource->datapoints, new_datapoint);
}

