
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <glib.h>

#include "internal.h"
#include "bionet-util.h"


int bionet_bdm_add_hab(bionet_bdm_t *bdm, const bionet_hab_t *hab) {
    if (bdm == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_add_hab(): NULL BDM passed in");
        return -1;
    }

    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_add_hab(): NULL HAB passed in");
        return -1;
    }

    if (bionet_bdm_get_hab_by_name(bdm, bionet_hab_get_name(hab)) != NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_add_hab(): HAB %s already present in BDM %s", bionet_hab_get_name(hab), bdm->id);
        return -1;
    }

    // ok, add the hab to the bdm's hab-list
    bdm->habs = g_slist_append(bdm->habs, (gpointer)hab);
    return 0;
}


