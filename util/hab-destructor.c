
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <stdlib.h>
#include "bionet-util.h"
#include "internal.h"

int bionet_hab_add_destructor(bionet_hab_t * hab, 
			      void (*destructor)(bionet_hab_t * hab, void * user_data),
			      void * user_data) {
    if (NULL == hab) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_hab_add_destructor: NULL HAB passed in.");
	return 1;
    }

    if (NULL == destructor) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_hab_add_destructor: NULL Destructor passed in.");
	return 1;
    }

    if (g_slist_find(hab->destructors, destructor)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	      "bionet_hab_add_destructor: trying to add a destructor which is already registered.");
	return 0;
    }

    bionet_hab_destructor_t * des = calloc(1, sizeof(bionet_hab_destructor_t));
    if (NULL == des) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_hab_add_destructor: Unable to allocate memory for destructor. %m");
	return 1;
    }

    des->destructor = destructor;
    des->user_data = user_data;

    hab->destructors = g_slist_append(hab->destructors, des);
    if (NULL == hab->destructors) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "bionet_hab_add_destructor: Unable to add destructor to destructor list.");
	return 1;
    }

    return 0;
} /* bionet_hab_add_destructor() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
