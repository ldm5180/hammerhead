
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <glib.h>

#include "internal.h"
#include "bionet-event.h"


bionet_event_t * bionet_event_new(const struct timeval *timestamp, const char * bdm_id, bionet_event_type_t type)
{
    bionet_event_t * event;
    
    event = calloc(1, sizeof(bionet_event_t));
    if( NULL == event ) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "bionet_event_new(): out of memory!");
	return NULL;
    }

    if(bdm_id) {
        event->bdm_id = strdup(bdm_id);

        if( NULL == event->bdm_id ) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "bionet_event_new(): out of memory!");
            bionet_event_free(event);
            return NULL;
        }
    }

    if(NULL == timestamp) {
	if (gettimeofday(&event->timestamp, NULL)) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "bionet_event_new: Failed to get time of day.");
            bionet_event_free(event);
            return NULL;
	}
    } else {
        event->timestamp.tv_sec = timestamp->tv_sec;
        event->timestamp.tv_usec = timestamp->tv_usec;
    }

    event->seq = -1;

    event->type = type;


    return event;
}

void bionet_event_free(bionet_event_t *event) {
    if (event == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_event_free(): NULL Event passed in");
        return;
    }

    if(event->ref) {
	event->ref = event->ref - 1;
	return;
    }

    free(event->bdm_id);
    free(event);
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
