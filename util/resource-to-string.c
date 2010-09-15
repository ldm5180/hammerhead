
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>

#include <glib.h>

#include "internal.h"
#include "bionet-util.h"


char *bionet_resource_value_to_string(const bionet_resource_t *resource, int datapoint_index) {
    const bionet_datapoint_t *datapoint;
    const bionet_value_t *value;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_value_as_string: NULL Resource passed in!");
        return NULL;
    }

    datapoint = bionet_resource_get_datapoint_by_index(resource, datapoint_index);
    if (datapoint == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_value_as_string: passed-in Resource has no Datapoint!");
        return NULL;
    }

    value = bionet_datapoint_get_value(datapoint);
    if (value == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_value_as_string: Datapoint of passed-in Resource has no Value!");
        return NULL;
    }

    return bionet_value_to_str(value);
}


char *bionet_resource_timestamp_to_string(const bionet_resource_t *resource, int datapoint_index) {
    const bionet_datapoint_t *datapoint;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_timestamp_as_string: NULL Resource passed in!");
        return NULL;
    }

    datapoint = bionet_resource_get_datapoint_by_index(resource, datapoint_index);
    if (datapoint == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_timestamp_as_string: passed-in Resource has no Datapoint!");
        return NULL;
    }

    return strdup(bionet_datapoint_timestamp_to_string(datapoint));
}

