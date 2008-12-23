
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <string.h>

#include <glib.h>

#ifdef BIONET_21_API
#include "internal.h"
#include "bionet-util-2.1.h"
#else
#include "bionet-util.h"
#endif

int bionet_resource_get_num_datapoints(const bionet_resource_t *resource) {
    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_num_datapoints(): NULL Resource passed in");
        errno = EINVAL;
        return -1;
    }
    return resource->datapoints->len;
}


bionet_datapoint_t *bionet_resource_get_datapoint_by_index(const bionet_resource_t *resource, unsigned int index) {
    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_datapoint_by_index(): NULL Resource passed in");
        errno = EINVAL;
        return NULL;
    }
    if (resource->datapoints->len <= index) return NULL;
    return g_ptr_array_index(resource->datapoints, index);
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
