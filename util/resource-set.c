
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "bionet-util.h"




int bionet_resource_set(bionet_resource_t *resource, const bionet_datapoint_value_t *value, const struct timeval *timestamp) {
    bionet_datapoint_t *d;

    d = bionet_resource_get_datapoint_by_index(resource, 0);
    if (d == NULL) {
        d = bionet_datapoint_new(resource, value, timestamp);
        if (d == NULL) {
            // an error has been logged
            return -1;
        }
        bionet_resource_add_existing_datapoint(resource, d);
        return 0;
    }

    bionet_datapoint_set_value(d, value);
    bionet_datapoint_set_timestamp(d, timestamp);
    return 0;
}


int bionet_resource_set_with_valuestr(bionet_resource_t *resource, const char *value_str, const struct timeval *timestamp) {
    bionet_datapoint_t *d;

    d = bionet_resource_get_datapoint_by_index(resource, 0);
    if (d == NULL) {
        d = bionet_datapoint_new_with_valuestr(resource, value_str, timestamp);
        if (d == NULL) {
            // an error has been logged
            return -1;
        }
        bionet_resource_add_existing_datapoint(resource, d);
        return 0;
    }

    bionet_datapoint_value_from_string(d, value_str);
    bionet_datapoint_set_timestamp(d, timestamp);
    return 0;
}

