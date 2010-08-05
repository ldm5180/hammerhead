
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>
#include <stdlib.h>

#include "internal.h"
#include "bionet-util.h"


int bionet_hab_get_num_events(const bionet_hab_t *hab) {
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_get_num_events(): NULL HAB passed in");
        return -1;
    }

    return g_slist_length(hab->events);
}


bionet_event_t *bionet_hab_get_event_by_index(const bionet_hab_t *hab, unsigned int index) {
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_get_event_by_index(): NULL HAB passed in");
        return NULL;
    }

    if (index > bionet_hab_get_num_events(hab)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_hab_get_num_events(): Index is greater than number of events available.");
	return NULL;
    }

    return g_slist_nth_data(hab->events, index);
}

int bionet_hab_add_event(bionet_hab_t *hab, const bionet_event_t *event) {
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_add_event(): NULL HAB passed in");
        return -1;
    }

    if (event == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_add_event(): NULL Event passed in");
        return -1;
    }

    // ok, add the event to the hab's event-list
    hab->events = g_slist_append(hab->events, (gpointer)event);
    return 0;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
