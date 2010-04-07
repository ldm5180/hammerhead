
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>
#include <errno.h>

#include "internal.h"
#include "bionet-util.h"


bionet_hab_t * bionet_bdm_remove_hab_by_type_id(bionet_bdm_t *bdm, const char * hab_type, const char *hab_id) {
    int i;


    if (bdm == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_remove_hab_by_type_id(): NULL BDM passed in");
	errno = EINVAL;
        return NULL;
    }

    if (hab_type == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_remove_hab_by_type_id(): NULL HAB-Type passed in");
	errno = EINVAL;
        return NULL;
    }

    if (hab_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_remove_hab_by_type_id(): NULL HAB-ID passed in");
	errno = EINVAL;
        return NULL;
    }

    for (i = 0; i < g_slist_length(bdm->habs); i ++) {
        bionet_hab_t *hab;

        hab = g_slist_nth_data(bdm->habs, i);
        if (strcmp(hab->id, hab_id) == 0) {
            bdm->habs = g_slist_remove(bdm->habs, hab);
            return hab;
        }
    }

    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_remove_hab_by_type_id(): HAB '%s' not found in BDM", hab_id);
    return NULL;
}


int bionet_bdm_remove_all_habs(bionet_bdm_t *bdm) {
    if (bdm == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_remove_all_habs(): NULL BDM passed in");
        return -1;
    }


    // remove all of this BDM's HABs
    do {
        bionet_hab_t *hab;

        hab = g_slist_nth_data(bdm->habs, 0);
        if (hab == NULL) break;  // done

        bdm->habs = g_slist_remove(bdm->habs, hab);

        // Don't free the HABs, sice there is a many-to-many relationship between habs and bdms.
        // The HABs memory is controlled by the global cache
    } while(1);

    return 0;
}


