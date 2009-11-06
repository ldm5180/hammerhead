
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>

#include "internal.h"
#include "bionet-util.h"


bionet_hab_t *bionet_bdm_get_hab_by_name(bionet_bdm_t *bdm, const char *hab_name) {
    int i;


    if (bdm == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_get_hab_by_name(): NULL HAB passed in");
        return NULL;
    }

    if (hab_name == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_get_hab_by_name(): NULL HAB-ID passed in");
        return NULL;
    }

    for (i = 0; i < g_slist_length(bdm->habs); i ++) {
        bionet_hab_t *hab;

        hab = g_slist_nth_data(bdm->habs, i);
        if (strcmp(bionet_hab_get_name(hab), hab_name) == 0) return hab;
    }

    return NULL;
}

bionet_hab_t *bionet_bdm_get_hab_by_type_id(bionet_bdm_t *bdm, const char *hab_type, const char *hab_id) {
    int i;


    if (bdm == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_get_hab_by_type_id(): NULL HAB passed in");
        return NULL;
    }

    if (hab_type == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_get_hab_by_type_id(): NULL HAB-Type passed in");
        return NULL;
    }

    if (hab_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_get_hab_by_type_id(): NULL HAB-ID passed in");
        return NULL;
    }

    for (i = 0; i < g_slist_length(bdm->habs); i ++) {
        bionet_hab_t *hab;

        hab = g_slist_nth_data(bdm->habs, i);
        if (strcmp(hab->id, hab_id) == 0 && strcmp(hab->type, hab_type) == 0) return hab;
    }

    return NULL;
}






int bionet_bdm_get_num_habs(const bionet_bdm_t *bdm) {
    if (bdm == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_get_num_habs(): NULL HAB passed in");
        return -1;
    }

    return g_slist_length(bdm->habs);
}


bionet_hab_t *bionet_bdm_get_hab_by_index(bionet_bdm_t *bdm, unsigned int index) {
    if (bdm == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_get_hab_by_index(): NULL HAB passed in");
        return NULL;
    }

    if (index > bionet_bdm_get_num_habs(bdm)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_bdm_get_num_habs(): Index is greater than number of habs available.");
	return NULL;
    }

    return g_slist_nth_data(bdm->habs, index);
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
