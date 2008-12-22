
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdlib.h>
#include <string.h>

#include "bionet-util.h"


const char * bionet_hab_get_type(bionet_hab_t *hab) {

    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_hab_get_type(): NULL HAB passed in!");
        return NULL;
    }

    return hab->type;
}


int bionet_hab_set_id(bionet_hab_t *hab, const char *id) {

    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_hab_set_id(): NULL HAB passed in!");
        return -1;
    }

    return hab->id;
}


bionet_hab_t* bionet_hab_new(const char* type, const char* id) {

    bionet_hab_t* hab;

    hab = calloc(1, sizeof(bionet_hab_t));
    if (hab == NULL) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "bionet_hab_new(): out of memory!");
	return NULL;
    }

    if (bionet_hab_set_type(hab, type) != 0) {
        // an error has been logged
        bionet_hab_free(hab);
        return NULL;
    }

    if (bionet_hab_set_id(hab, id) != 0) {
        // an error has been logged
        bionet_hab_free(hab);
        return NULL;
    }

    return hab;
}

