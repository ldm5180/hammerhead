
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>

#include "internal.h"
#include "bionet-util.h"


int bionet_hab_matches_type_and_id(const bionet_hab_t *hab, const char *type, const char *id) {
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_matches_type_and_id: NULL hab passed in!");
        return 0;
    }

    if (!bionet_name_component_matches(hab->type, type)) return 0;
    if (!bionet_name_component_matches(hab->id, id)) return 0;

    return 1;
}


