
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


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

    bionet_hab_remove_all_nodes(hab);

    free(hab);
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
