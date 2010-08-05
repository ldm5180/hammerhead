
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>

#include <glib.h>

#include "internal.h"
#include "bionet-event.h"
#include <stdlib.h>

const struct timeval * bionet_event_get_timestamp(const bionet_event_t * event)
{
    return &event->timestamp;
}

char * bionet_event_get_timestamp_as_str(const bionet_event_t * event)
{
    char usec_str[10];
    struct tm *tm;
    int r;

    int size = 64;
    char * time_str = malloc(size);
    if(NULL == time_str) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "Out of memory");
        return NULL;
    }

    // 
    // sanity tests
    //

    if (event == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "%s(): NULL event passed in", __FUNCTION__);
        goto error;
    }

    tm = gmtime((time_t *)&event->timestamp.tv_sec);
    if (tm == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "%s(): error with gmtime: %s", __FUNCTION__,
	      strerror(errno));
        goto error;
    }

    r = strftime(time_str, size, "%Y-%m-%d %H:%M:%S", tm);
    if (r <= 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "%s(): error with strftime: %s", __FUNCTION__,
	      strerror(errno));
        goto error;
    }

    r = snprintf(usec_str, sizeof(usec_str), ".%06ld", 
            (long)event->timestamp.tv_usec);
    if (r >= sizeof(usec_str)) {
        // this should never happen, but it keeps Coverity happy
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "%s(): usec_str too small?!", __FUNCTION__);
        goto error;
    }

    // sanity check destination memory size available
    if ((strlen(usec_str) + 1 + strlen(time_str)) > size) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "%s(): time_str too small?!", __FUNCTION__);
        goto error;
    }
    strncat(time_str, usec_str, strlen(usec_str));

    return time_str;

error:
    strncpy(time_str, "invalid time", size);
    time_str[size-1] = '\0';
    return time_str;
}

/*
bionet_bdm_t * bionet_event_get_bdm(const bionet_event_t * event)
{
}
*/


const char * bionet_event_get_bdm_id(const bionet_event_t * event)
{
    return event->bdm_id;
}


bionet_event_type_t bionet_event_get_type(const bionet_event_t * event)
{
    return event->type;
}

int64_t bionet_event_get_seq(const bionet_event_t * event)
{
    return event->seq;
}

void bionet_event_set_seq(bionet_event_t * event, int64_t seq)
{
    event->seq = seq;
}



