
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <glib.h>

#include "bionet-util.h"
#include "internal.h"



bionet_datapoint_t * bionet_datapoint_dup(bionet_datapoint_t * datapoint) {
    bionet_datapoint_t *new;


    //
    // sanity checking
    //

    if (datapoint == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_dup(): NULL datapoint passed in");
        return NULL;
    }

    new = (bionet_datapoint_t *)calloc(1, sizeof(bionet_datapoint_t));
    if (NULL == new) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "bionet_datapoint_dup(): out of memory!");
        return NULL;
    }

    new->value = bionet_value_dup(bionet_datapoint_get_resource(datapoint), bionet_datapoint_get_value(datapoint));
    if(NULL == new->value) {
	free(new);
	return NULL;
    }

    bionet_datapoint_set_timestamp(new, bionet_datapoint_get_timestamp(datapoint));

    new->dirty = 0;

    return new;
} /* bionet_datapoint_dup() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
