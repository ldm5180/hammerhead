
// Copyright (c) 2008-2010, Regents of the University of Colorado.
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

    bionet_hab_remove_all_nodes(hab);

    // free all the events
    while (hab->events != NULL) {
        bionet_event_t *event = hab->events->data;

        hab->events = g_slist_remove(hab->events, event);
        bionet_event_free(event);
    }

    if (hab->recording_bdm != NULL) {
        free(hab->recording_bdm);
    }

    g_slist_foreach(hab->destructors, bionet_hab_destruct, hab);

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

    free(hab);
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
