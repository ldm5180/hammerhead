
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <stdlib.h>
#include "bionet-util.h"
#include "internal.h"

int bionet_datapoint_add_destructor(bionet_datapoint_t * datapoint, 
			      void (*destructor)(bionet_datapoint_t * datapoint, void * user_data),
			      void * user_data) {
    if (NULL == datapoint) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_datapoint_add_destructor: NULL DATAPOINT passed in.");
	return 1;
    }

    if (NULL == destructor) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_datapoint_add_destructor: NULL Destructor passed in.");
	return 1;
    }

    if (g_slist_find(datapoint->destructors, destructor)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	      "bionet_datapoint_add_destructor: trying to add a destructor which is already registered.");
	return 0;
    }

    bionet_datapoint_destructor_t * des = calloc(1, sizeof(bionet_datapoint_destructor_t));
    if (NULL == des) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_datapoint_add_destructor: Unable to allocate memory for destructor. %m");
	return 1;
    }

    des->destructor = destructor;
    des->user_data = user_data;

    datapoint->destructors = g_slist_append(datapoint->destructors, des);
    if (NULL == datapoint->destructors) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "bionet_datapoint_add_destructor: Unable to add destructor to destructor list.");
	return 1;
    }

    return 0;
} /* bionet_datapoint_add_destructor() */


void bionet_datapoint_destruct(gpointer des, gpointer datapoint) {
    bionet_datapoint_destructor_t * destructor = (bionet_datapoint_destructor_t *)des;
    if (NULL == destructor) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "bionet_datapoint_destruct: NULL destructor func passed in.");
	return;
    }

    if (destructor) {
	destructor->destructor((bionet_datapoint_t *)datapoint, (void *)destructor->user_data);
    }

    return;
} /* bionet_datapoint_destruct() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
