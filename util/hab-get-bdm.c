
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>

#include "internal.h"
#include "bionet-util.h"


bionet_bdm_t *bionet_hab_get_bdm_by_id(bionet_hab_t *hab, const char *bdm_id) {
    int i;


    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_get_bdm_by_id(): NULL HAB passed in");
        return NULL;
    }

    if (bdm_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_get_bdm_by_id(): NULL BDM-ID passed in");
        return NULL;
    }

    for (i = 0; i < g_slist_length(hab->bdms); i ++) {
        bionet_bdm_t *bdm;

        bdm = g_slist_nth_data(hab->bdms, i);
        if (strcmp(bdm->id, bdm_id) == 0) return bdm;
    }

    return NULL;
}




int bionet_hab_get_num_bdms(const bionet_hab_t *hab) {
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_get_num_bdms(): NULL HAB passed in");
        return -1;
    }

    return g_slist_length(hab->bdms);
}


bionet_bdm_t *bionet_hab_get_bdm_by_index(bionet_hab_t *hab, unsigned int index) {
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_get_bdm_by_index(): NULL HAB passed in");
        return NULL;
    }

    if (index > bionet_hab_get_num_bdms(hab)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_hab_get_num_bdms(): Index is greater than number of bdms available.");
	return NULL;
    }

    return g_slist_nth_data(hab->bdms, index);
}

const char * bionet_hab_get_recording_bdm(bionet_hab_t *hab)
{
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_get_recording_bdm(): NULL HAB passed in");
        return NULL;
    }

    return hab->recording_bdm;
}

void bionet_hab_set_recording_bdm(bionet_hab_t *hab, const char * bdm_id)
{
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_get_recording_bdm(): NULL HAB passed in");
        return;
    }

    hab->recording_bdm = strdup(bdm_id);
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
