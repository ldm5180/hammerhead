
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include <glib.h>

#include "bionet-util.h"
#include "internal.h"

int bionet_timeval_compare(const struct timeval * tva, const struct timeval * tvb) {
    if(tva->tv_sec < tvb->tv_sec) return -1;
    if(tva->tv_sec > tvb->tv_sec) return 1;
    if(tva->tv_usec < tvb->tv_usec) return -1;
    if(tva->tv_usec > tvb->tv_usec) return 1;
    return 0;
}


// FIXME: better error handling here - return NULL and set errno if there are problems
const char *bionet_datapoint_timestamp_to_string(const bionet_datapoint_t *datapoint) {
    static char time_str[200];

    char usec_str[10];
    struct tm *tm;
    int r;

    // 
    // sanity tests
    //

    if (datapoint == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_datapoint_timestamp_to_string_human_readable(): NULL datapoint passed in");
        return "invalid time";
    }


    tm = gmtime((time_t *)&datapoint->timestamp.tv_sec);
    if (tm == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_datapoint_timestamp_to_string_human_readable(): error with gmtime: %s", 
	      strerror(errno));
        return "invalid time";
    }

    r = strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm);
    if (r <= 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_datapoint_timestamp_to_string_human_readable(): error with strftime: %s", 
	      strerror(errno));
        return "invalid time";
    }

    r = snprintf(usec_str, sizeof(usec_str), ".%06ld", (long)datapoint->timestamp.tv_usec);
    if (r >= sizeof(usec_str)) {
        // this should never happen, but it keeps Coverity happy
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_datapoint_timestamp_to_string_human_readable(): usec_str too small?!");
        return "invalid time";
    }

    // sanity check destination memory size available
    if ((strlen(usec_str) + 1 + strlen(time_str)) > sizeof(time_str)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_datapoint_timestamp_to_string_human_readable(): time_str too small?!");
        return "invalid time";
    }
    strncat(time_str, usec_str, strlen(usec_str));

    return time_str;
}




void bionet_datapoint_set_timestamp(bionet_datapoint_t *datapoint, const struct timeval *new_timestamp) {
    if (datapoint == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_set_timestamp(): NULL datapoint passed in");
        return;
    }

    if (new_timestamp == NULL) {
        int r;

        r = gettimeofday(&datapoint->timestamp, NULL);
        if (r < 0) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_datapoint_set_timestamp(): error getting time: %s", 
		  strerror(errno));
            return;
        }
    } else {
        datapoint->timestamp.tv_sec  = new_timestamp->tv_sec;
        datapoint->timestamp.tv_usec = new_timestamp->tv_usec;
    }

    datapoint->dirty = 1;

    return;
}


struct timeval * bionet_datapoint_get_timestamp(bionet_datapoint_t *datapoint)
{
    if (NULL == datapoint)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_datapoint_get_timestamp(): NULL datapoint passed in");
	errno = EINVAL;
	return NULL;
    }

    return &datapoint->timestamp;
}


struct timeval bionet_timeval_subtract(struct timeval * x, struct timeval *y) {
    struct timeval result;

    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec) {
	int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
	y->tv_usec -= 1000000 * nsec;
	y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
	int nsec = (x->tv_usec - y->tv_usec) / 1000000;
	y->tv_usec += 1000000 * nsec;
	y->tv_sec -= nsec;
    }
    
    /* Compute the time remaining to wait.
       tv_usec is certainly positive. */
    result.tv_sec = x->tv_sec - y->tv_sec;
    result.tv_usec = x->tv_usec - y->tv_usec;
    
    /* Return 1 if result is negative. */
    return result;
} /* bionet_timeval_subtract() */

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
