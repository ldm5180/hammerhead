
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>

#include "internal.h"
#include "bionet-util.h"


void bionet_datapoint_set_user_data(bionet_datapoint_t *datapoint, const void *user_data) {
    if (datapoint == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_set_user_data(): NULL datapoint passed in!");
        return;
    }

    datapoint->user_data = user_data;
}


void *bionet_datapoint_get_user_data(const bionet_datapoint_t *datapoint) {
    if (datapoint == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_get_user_data(): NULL datapoint passed in!");
        return NULL;
    }

    return (void *)datapoint->user_data;
}


