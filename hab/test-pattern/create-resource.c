
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <glib.h>


#include "test-pattern-hab.h"

struct resource_info_t *create_resource(char *id, 
    bionet_resource_data_type_t data_type, 
    bionet_resource_flavor_t flavor,
    char *value) {

    struct resource_info_t *res;

    res = calloc(1, sizeof(struct resource_info_t));
    if (res == NULL) {
        g_log("", G_LOG_LEVEL_ERROR, "calloc failed (allocating resource_info): %s\n", strerror(errno));
    }

    strcpy(res->id, id);
    res->flavor = flavor;
    res->data_type = data_type;
    res->value = value;
    res->has_value = TRUE;

    free(id);
    
    return res;
}

struct resource_info_t *create_empty_resource(char *id, 
    bionet_resource_data_type_t data_type, 
    bionet_resource_flavor_t flavor) {

    struct resource_info_t *res;

    res = calloc(1, sizeof(struct resource_info_t));
    if (res == NULL) {
        g_log("", G_LOG_LEVEL_ERROR, "calloc failed (allocating resource_info): %s\n", strerror(errno));
        exit(1);
    }

    strcpy(res->id, id);
    res->flavor = flavor;
    res->data_type = data_type;
    res->has_value = FALSE;
    res->value = NULL;

    free(id);
    
    return res;
}
