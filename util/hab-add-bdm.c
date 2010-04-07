
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <glib.h>

#include "internal.h"
#include "bionet-util.h"


int bionet_hab_add_bdm(bionet_hab_t *hab, const bionet_bdm_t *bdm) {
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_add_bdm(): NULL HAB passed in");
        return -1;
    }

    if (bdm == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_add_bdm(): NULL BDM passed in");
        return -1;
    }

    if (bionet_hab_get_bdm_by_id(hab, bdm->id) != NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_add_bdm(): BDM %s already present in HAB %s.%s", bdm->id, hab->type, hab->id);
        return -1;
    }

    // ok, add the bdm to the hab's bdm-list
    hab->bdms = g_slist_append(hab->bdms, (gpointer)bdm);
    return 0;
}


