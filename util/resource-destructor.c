
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <stdlib.h>
#include "bionet-util.h"
#include "internal.h"

int bionet_resource_add_destructor(bionet_resource_t * resource, 
			      void (*destructor)(bionet_resource_t * resource, void * user_data),
			      void * user_data) {
    if (NULL == resource) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_add_destructor: NULL RESOURCE passed in.");
	return 1;
    }

    if (NULL == destructor) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_add_destructor: NULL Destructor passed in.");
	return 1;
    }

    if (g_slist_find(resource->destructors, destructor)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	      "bionet_resource_add_destructor: trying to add a destructor which is already registered.");
	return 0;
    }

    bionet_resource_destructor_t * des = calloc(1, sizeof(bionet_resource_destructor_t));
    if (NULL == des) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_add_destructor: Unable to allocate memory for destructor. %m");
	return 1;
    }

    des->destructor = destructor;
    des->user_data = user_data;

    resource->destructors = g_slist_append(resource->destructors, des);
    if (NULL == resource->destructors) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "bionet_resource_add_destructor: Unable to add destructor to destructor list.");
	return 1;
    }

    return 0;
} /* bionet_resource_add_destructor() */


void bionet_resource_destruct(gpointer des, gpointer resource) {
    bionet_resource_destructor_t * destructor = (bionet_resource_destructor_t *)des;
    if (NULL == destructor) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "bionet_resource_destruct: NULL destructor func passed in.");
	return;
    }

    if (destructor) {
	destructor->destructor((bionet_resource_t *)resource, (void *)destructor->user_data);
    }

    return;
} /* bionet_resource_destruct() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
