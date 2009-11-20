
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>

#include <glib.h>

#include "internal.h"
#include "bionet-util.h"


void bionet_hab_free(bionet_hab_t *hab) {
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_free(): NULL HAB passed in");
        return;
    }

    if (hab->user_data != NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_free(): passed-in HAB has non-NULL user_data pointer, ignoring");
    }

    if (hab->type != NULL) {
        free(hab->type);
    }

    if (hab->id != NULL) {
        free(hab->id);
    }

    if (hab->name != NULL) {
        free(hab->name);
    }

    if (hab->recording_bdm != NULL) {
        free(hab->recording_bdm);
    }


    bionet_hab_remove_all_nodes(hab);

    free(hab);
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
