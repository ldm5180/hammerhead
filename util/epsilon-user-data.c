
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>

#include "internal.h"
#include "bionet-util.h"


void bionet_epsilon_set_user_data(bionet_epsilon_t *epsilon, const void *user_data) {
    if (epsilon == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_epsilon_set_user_data(): NULL epsilon passed in!");
        return;
    }

    epsilon->user_data = user_data;
}


void *bionet_epsilon_get_user_data(const bionet_epsilon_t *epsilon) {
    if (epsilon == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_epsilon_get_user_data(): NULL epsilon passed in!");
        return NULL;
    }

    return (void *)epsilon->user_data;
}


