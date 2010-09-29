
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>

#include "internal.h"
#include "bionet-util.h"


void bionet_value_set_user_data(bionet_value_t *value, const void *user_data) {
    if (value == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_value_set_user_data(): NULL value passed in!");
        return;
    }

    value->user_data = user_data;
}


void *bionet_value_get_user_data(const bionet_value_t *value) {
    if (value == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_value_get_user_data(): NULL value passed in!");
        return NULL;
    }

    return (void *)value->user_data;
}


