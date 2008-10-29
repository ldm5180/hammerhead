
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <string.h>

#include "bionet-util.h"


const char *bionet_resource_flavor_to_string(bionet_resource_flavor_t flavor) {
    static char *resource_flavor_to_string_array[] = {
        "(invalid)",
        "Sensor",
        "Actuator",
        "Parameter"
    };

    if (
        (flavor < BIONET_RESOURCE_FLAVOR_MIN) ||
        (flavor > BIONET_RESOURCE_FLAVOR_MAX)
    ) {
        return resource_flavor_to_string_array[0];  // invalid
    }

    return resource_flavor_to_string_array[flavor+1];
}


bionet_resource_flavor_t bionet_resource_flavor_from_string(const char *flavor_string) {
    if (flavor_string == NULL) return BIONET_RESOURCE_FLAVOR_INVALID;

    if (strcasecmp(flavor_string, "sensor") == 0) {
        return BIONET_RESOURCE_FLAVOR_SENSOR;

    } else if (strcasecmp(flavor_string, "actuator") == 0) {
        return BIONET_RESOURCE_FLAVOR_ACTUATOR;

    } else if (strcasecmp(flavor_string, "parameter") == 0) {
        return BIONET_RESOURCE_FLAVOR_PARAMETER;

    }

    return BIONET_RESOURCE_FLAVOR_INVALID;
}

