
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <glib.h>

#include "bionet-util.h"
#include "internal.h"



bionet_datapoint_t *bionet_datapoint_new(
    bionet_resource_t *resource,
    bionet_value_t *value,
    const struct timeval *timestamp
) {
    bionet_datapoint_t *d;


    //
    // sanity checking
    //

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_new(): NULL resource passed in");
        return NULL;
    }

    if (value == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_new(): NULL value passed in");
	assert(0);
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

    d->value = value;

    bionet_datapoint_set_timestamp(d, timestamp);

    d->dirty = 1;

    return d;
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
