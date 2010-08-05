
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>
#include <stdlib.h>

#include "internal.h"
#include "bionet-util.h"


int bionet_datapoint_get_num_events(const bionet_datapoint_t *datapoint) {
    if (datapoint == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_get_num_events(): NULL Datapoint passed in");
        return -1;
    }

    return g_slist_length(datapoint->events);
}


bionet_event_t *bionet_datapoint_get_event_by_index(const bionet_datapoint_t *datapoint, unsigned int index) {
    if (datapoint == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_get_event_by_index(): NULL Datapoint passed in");
        return NULL;
    }

    if (index > bionet_datapoint_get_num_events(datapoint)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_datapoint_get_num_events(): Index is greater than number of events available.");
	return NULL;
    }

    return g_slist_nth_data(datapoint->events, index);
}

int bionet_datapoint_add_event(bionet_datapoint_t *datapoint, const bionet_event_t *event) {
    if (datapoint == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_add_event(): NULL Datapoint passed in");
        return -1;
    }

    if (event == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_add_event(): NULL Event passed in");
        return -1;
    }

    // ok, add the event to the datapoint's event-list
    datapoint->events = g_slist_append(datapoint->events, (gpointer)event);
    return 0;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
