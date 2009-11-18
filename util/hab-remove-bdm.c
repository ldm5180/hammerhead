
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>
#include <errno.h>

#include "internal.h"
#include "bionet-util.h"


bionet_bdm_t * bionet_hab_remove_bdm_by_id(bionet_hab_t *hab, const char *bdm_id) {
    int i;


    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_remove_bdm_by_id(): NULL HAB passed in");
	errno = EINVAL;
        return NULL;
    }

    if (bdm_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_remove_bdm_by_id(): NULL BDM-ID passed in");
	errno = EINVAL;
        return NULL;
    }

    for (i = 0; i < g_slist_length(hab->bdms); i ++) {
        bionet_bdm_t *bdm;

        bdm = g_slist_nth_data(hab->bdms, i);
        if (strcmp(bdm->id, bdm_id) == 0) {
            hab->bdms = g_slist_remove(hab->bdms, bdm);
            return bdm;
        }
    }

    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_remove_bdm_by_id(): BDM '%s' not found in HAB", bdm_id);
    return NULL;
}


int bionet_hab_remove_all_bdms(bionet_hab_t *hab) {
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_remove_all_bdms(): NULL HAB passed in");
        return -1;
    }


    // remove all of this HAB's BDMs
    do {
        bionet_bdm_t *bdm;

        bdm = g_slist_nth_data(hab->bdms, 0);
        if (bdm == NULL) break;  // done

        hab->bdms = g_slist_remove(hab->bdms, bdm);

        // Don't free the BDM. This list just cross-links the BDMs and HABs since there 
        // is a many-to-many relationship. The global cache owns the memory for HABs and BDMs
    } while(1);

    return 0;
}


