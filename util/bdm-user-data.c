
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>

#include "internal.h"
#include "bionet-util.h"


void bionet_bdm_set_user_data(bionet_bdm_t *bdm, const void *user_data) {
    if (bdm == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_set_user_data(): NULL bdm passed in!");
        return;
    }

    bdm->user_data = user_data;
}


void *bionet_bdm_get_user_data(const bionet_bdm_t *bdm) {
    if (bdm == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_get_user_data(): NULL bdm passed in!");
        return NULL;
    }

    return (void *)bdm->user_data;
}


