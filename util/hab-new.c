
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdlib.h>
#include <string.h>

#include "bionet-util.h"


bionet_hab_t* bionet_hab_new(
    const char* type,
    const char* id
) {
    bionet_hab_t* hab;

    hab = calloc(1, sizeof(bionet_hab_t));
    if (hab == NULL) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "bionet_hab_new(): out of memory!");
	return NULL;
    }

    if (type != NULL) {
        hab->type = strdup(type);
        if (hab->type == NULL) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "bionet_hab_new(): out of memory!");
            free(hab);
            return NULL;
        }
    }

    if (id != NULL) {
        hab->id = strdup(id);
        if (hab->id == NULL) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "bionet_hab_new(): out of memory!");
            if (hab->type != NULL) free(hab->type);
            free(hab);
            return NULL;
        }
    }

    return hab;
}

