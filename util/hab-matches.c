
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <string.h>

#ifdef BIONET_21_API
#include "internal.h"
#include "bionet-util-2.1.h"
#else
#include "bionet-util.h"
#endif

int bionet_hab_matches_type_and_id(const bionet_hab_t *hab, const char *type, const char *id) {
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_matches_type_and_id: NULL hab passed in!");
        return 0;
    }

    if (!bionet_name_component_matches(hab->type, type)) return 0;
    if (!bionet_name_component_matches(hab->id, id)) return 0;

    return 1;
}


