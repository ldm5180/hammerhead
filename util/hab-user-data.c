
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>

#include "internal.h"
#include "bionet-util.h"


void bionet_hab_set_user_data(bionet_hab_t *hab, const void *user_data) {
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_set_user_data(): NULL hab passed in!");
        return;
    }

    hab->user_data = user_data;
}


void *bionet_hab_get_user_data(const bionet_hab_t *hab) {
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_get_user_data(): NULL hab passed in!");
        return NULL;
    }

    return (void *)hab->user_data;
}


