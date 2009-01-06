
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <string.h>

#include "internal.h"
#include "bionet-util.h"


void bionet_node_set_user_data(bionet_node_t *node, const void *user_data) {
    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_set_user_data(): NULL node passed in!");
        return;
    }

    node->user_data = user_data;
}


void *bionet_node_get_user_data(const bionet_node_t *node) {
    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_get_user_data(): NULL node passed in!");
        return NULL;
    }

    return (void *)node->user_data;
}


