
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>

#include "internal.h"
#include "bionet-util.h"


void bionet_resource_set_user_data(bionet_resource_t *resource, const void *user_data) {
    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_set_user_data(): NULL resource passed in!");
        return;
    }

    resource->user_data = user_data;
}


void *bionet_resource_get_user_data(const bionet_resource_t *resource) {
    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_user_data(): NULL resource passed in!");
        return NULL;
    }

    return (void *)resource->user_data;
}


