
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>

#include <glib.h>

#include "bionet-util.h"
#include "internal.h"


static void resource_destroy (gpointer data, gpointer user_data);


void bionet_resource_free(bionet_resource_t *resource) {

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_free(): NULL Resource passed in");
        return;
    }

    g_slist_foreach(resource->destructors,
		    resource_destroy,
		    resource);

    if (resource->user_data != NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_free() called with Resource %s, user_data is not NULL, leaking memory now", resource->id);
    }

    // free all the datapoints
    while(resource->datapoints->len > 0) {
        bionet_datapoint_t *d = g_ptr_array_remove_index_fast(resource->datapoints, 0);
        bionet_datapoint_free(d);
    }
    g_ptr_array_free(resource->datapoints, TRUE);

    if (resource->id != NULL) {
        free(resource->id);
    }

    if (resource->name != NULL) {
        free(resource->name);
    }

    if (resource->local_name != NULL) {
        free(resource->local_name);
    }

    free(resource);
}


static void resource_destroy (gpointer data, gpointer user_data) {
    bionet_resource_destructor_t * des = (bionet_resource_destructor_t *)data;
    bionet_resource_t * resource = (bionet_resource_t *)user_data;

    if (NULL == des) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "resource_destroy: NULL destructor passed in.");
	return;
    }

    if (NULL == resource) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "resource_destroy: NULL resource passed in.");
	return;
    }

    des->destructor(resource, des->user_data);
} /* resource_destroy() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
