
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>

#include <glib.h>

#include "internal.h"
#include "bionet-util.h"


void bionet_bdm_free(bionet_bdm_t *bdm) {
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

    g_ptr_array_free(bdm->hab_list, 1);

    free(bdm);
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
