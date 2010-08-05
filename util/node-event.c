
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>
#include <stdlib.h>

#include "internal.h"
#include "bionet-util.h"


int bionet_node_get_num_events(const bionet_node_t *node) {
    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_get_num_events(): NULL Datapoint passed in");
        return -1;
    }

    return g_slist_length(node->events);
}


bionet_event_t *bionet_node_get_event_by_index(const bionet_node_t *node, unsigned int index) {
    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_get_event_by_index(): NULL Datapoint passed in");
        return NULL;
    }

    if (index > bionet_node_get_num_events(node)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_node_get_num_events(): Index is greater than number of events available.");
	return NULL;
    }

    return g_slist_nth_data(node->events, index);
}

int bionet_node_add_event(bionet_node_t *node, const bionet_event_t *event) {
    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_add_event(): NULL Datapoint passed in");
        return -1;
    }

    if (event == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_add_event(): NULL Event passed in");
        return -1;
    }

    // ok, add the event to the node's event-list
    node->events = g_slist_append(node->events, (gpointer)event);
    return 0;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
