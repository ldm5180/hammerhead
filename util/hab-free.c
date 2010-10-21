
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>

#include <glib.h>

#include "internal.h"
#include "bionet-util.h"


static void hab_destroy (gpointer data, gpointer user_data);


void bionet_hab_free(bionet_hab_t *hab) {
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_free(): NULL HAB passed in");
        return;
    }

    if(hab->ref) {
	hab->ref = hab->ref - 1;
	return;
    }

    bionet_hab_remove_all_nodes(hab);

    /* run all the destructors */
    g_slist_foreach(hab->destructors,
		    hab_destroy,
		    hab);


    /* free all the destructors */
    while (hab->destructors) {
	bionet_hab_destructor_t * des = hab->destructors->data;
	hab->destructors = g_slist_remove(hab->destructors, des);
	free(des);
    }

    // free all the events
    while (hab->events != NULL) {
        bionet_event_t *event = hab->events->data;

        hab->events = g_slist_remove(hab->events, event);
        bionet_event_free(event);
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

    free(hab);
} /* bionet_hab_free() */


static void hab_destroy (gpointer data, gpointer user_data) {
    bionet_hab_destructor_t * des = (bionet_hab_destructor_t *)data;
    bionet_hab_t * hab = (bionet_hab_t *)user_data;

    if (NULL == des) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "hab_destroy: NULL destructor passed in.");
	return;
    }

    if (NULL == hab) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "hab_destroy: NULL resource passed in.");
	return;
    }

    des->destructor(hab, des->user_data);
} /* hab_destroy() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
