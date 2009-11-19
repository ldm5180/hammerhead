
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>

#include <glib.h>

#include "internal.h"
#include "bionet-util.h"


void bionet_bdm_free(bionet_bdm_t *bdm) {
    GSList * li;

    if (bdm == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_free(): NULL HAB passed in");
        return;
    }

    if (bdm->user_data != NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_free(): passed-in HAB has non-NULL user_data pointer, ignoring");
    }

    if (bdm->id != NULL) {
        free(bdm->id);
    }

    // remove all of this BDMs HABs where this bdm is the last one in the hab
    for(li = bdm->habs; li != NULL; li = li->next) {
        bionet_hab_t *hab;

        hab = li->data;


        // This BDM will get deleted, so remove the links to it...
        hab->bdms = g_slist_remove(hab->bdms, bdm);

        // Don't free the hab if there is another BDM linked to it..
        if(bionet_hab_get_num_bdms(hab) == 0) {
            bionet_hab_free(hab);
        }

    }

    if(bdm->habs) {
        g_slist_free(bdm->habs);
    }

    free(bdm);
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
