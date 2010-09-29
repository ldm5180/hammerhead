
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>
#include <errno.h>

#include <glib.h>

#include "bionet-util.h"
#include "internal.h"


static void datapoint_destroy (gpointer data, gpointer user_data);


void bionet_datapoint_free(bionet_datapoint_t *d) {
    if (NULL == d)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_datapoint_free(): NULL Datapoint passed in");
	errno = EINVAL;
	return;
    }

    if (d->ref) {
	d->ref = d->ref - 1;
	return;
    }

    /* run all the destructors */
    g_slist_foreach(d->destructors,
		    datapoint_destroy,
		    d);

    /* free all the destructors */
    while (d->destructors) {
	bionet_datapoint_destructor_t * des = d->destructors->data;
	d->destructors = g_slist_remove(d->destructors, des);
	free(des);
    }

    // free all the events
    while (d->events != NULL) {
        bionet_event_t *event = d->events->data;

        d->events = g_slist_remove(d->events, event);
        bionet_event_free(event);
    }

    if (NULL != d->value) { 
	bionet_value_free(d->value);
    }
    free(d);
} /* bionet_datapoint_free() */


static void datapoint_destroy (gpointer data, gpointer user_data) {
    bionet_datapoint_destructor_t * des = (bionet_datapoint_destructor_t *)data;
    bionet_datapoint_t * dp = (bionet_datapoint_t *)user_data;

    if (NULL == des) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "datapoint_destroy: NULL destructor passed in.");
	return;
    }

    if (NULL == dp) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "datapoint_destroy: NULL datapoint passed in.");
	return;
    }

    des->destructor(dp, des->user_data);
} /* resource_destroy() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
