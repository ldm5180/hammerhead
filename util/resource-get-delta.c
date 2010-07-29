
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>

#include <glib.h>

#include "internal.h"
#include "bionet-util.h"


const struct timeval * bionet_resource_get_delta(const bionet_resource_t *resource) {
    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_delta(): NULL Resource passed in");
        errno = EINVAL;
        return NULL;
    }

    if (resource->delta.tv_sec || resource->delta.tv_usec) {
	return &resource->delta;
    }

    return NULL;
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
