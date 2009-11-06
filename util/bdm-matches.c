
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>

#include "internal.h"
#include "bionet-util.h"


int bionet_bdm_matches_id(const bionet_bdm_t *bdm, const char *id) {
    if (bdm == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_matches_id: NULL bdm passed in!");
        return 0;
    }

    if (!bionet_name_component_matches(bdm->id, id)) return 0;

    return 1;
}


