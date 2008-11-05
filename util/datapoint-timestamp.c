
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include <glib.h>

#include "bionet-util.h"


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
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_timestamp_to_string_human_readable(): NULL datapoint passed in");
        return "invalid time";
    }


    tm = gmtime((time_t *)&datapoint->timestamp.tv_sec);
    if (tm == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_timestamp_to_string_human_readable(): error with gmtime: %s", strerror(errno));
        return "invalid time";
    }

    r = strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm);
    if (r <= 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_timestamp_to_string_human_readable(): error with strftime: %s", strerror(errno));
        return "invalid time";
    }

    r = snprintf(usec_str, sizeof(usec_str), ".%06ld", (long)datapoint->timestamp.tv_usec);
    if (r >= sizeof(usec_str)) {
        // this should never happen, but it keeps Coverity happy
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_timestamp_to_string_human_readable(): usec_str too small?!");
        return "invalid time";
    }

    // sanity check destination memory size available
    if ((strlen(usec_str) + 1 + strlen(time_str)) > sizeof(time_str)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_timestamp_to_string_human_readable(): time_str too small?!");
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
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_set_timestamp(): error getting time: %s", strerror(errno));
            return;
        }
    } else {
        datapoint->timestamp.tv_sec  = new_timestamp->tv_sec;
        datapoint->timestamp.tv_usec = new_timestamp->tv_usec;
    }

    datapoint->dirty = 1;

    return;
}




#if 0
// FIXME: better error handling here - return NULL and set errno if there are problems
const char *bionet_resource_time_to_string(const bionet_resource_t *resource) {
    static char time_str[100];
    int r;


    // 
    // sanity checks
    //

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_time_to_string(): NULL resource passed in");
        return "invalid time";
    }


    r = snprintf(time_str, sizeof(time_str), "%d.%06d", (int)resource->time.tv_sec, (int)resource->time.tv_usec);
    if (r < 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_time_to_string(): error with snprintf: %s", strerror(errno));
        return "invalid time";
    }
    if (r >= sizeof(time_str)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_time_to_string(): time_str too small?!");
        return "invalid time";
    }

    return time_str;
}




int bionet_resource_time_encode(const bionet_resource_t *resource, void **p, int *size) {
    static char time_buf[8];
    int32_t n;

    n = g_htonl((int32_t)resource->time.tv_sec);
    memcpy(&time_buf[0], &n, 4);

    n = g_htonl((int32_t)resource->time.tv_usec);
    memcpy(&time_buf[4], &n, 4);

    *p = time_buf;
    *size = 8;

    return 0;
}


int bionet_resource_time_decode(bionet_resource_t *resource, const void *p, int size) {
    if (resource == NULL) {
        errno = EINVAL;
        return 0;
    }

    if (size < 8) {
        errno = EINVAL;
        return 0;
    }

    resource->time.tv_sec  = (int)g_ntohl(((uint32_t *)p)[0]);
    resource->time.tv_usec = (int)g_ntohl(((uint32_t *)p)[1]);

    return 8;
}


int bionet_resource_time_from_string(const char *time_str, bionet_resource_t *resource) {
    int r;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_set_resource_time_from_string(): NULL resource passed in");
        errno = EINVAL;
        return -1;
    }

    if (time_str == NULL) {
        r = gettimeofday(&resource->time, NULL);
        if (r < 0) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_set_resource_time_from_string(): error getting time: %s", strerror(errno));
            return -1;
        }

        resource->dirty = 1;

        return 0;
    }


    // first try full time
    {
        time_t t;
        struct tm tm;
        int usec;

        memset(&tm, 0, sizeof(tm));
        tm.tm_isdst = -1;  // dont try to do DST

        r = sscanf(
            time_str,
            "%d-%d-%d %d:%d:%d.%d", 
            &tm.tm_year,
            &tm.tm_mon,
            &tm.tm_mday,
            &tm.tm_hour,
            &tm.tm_min,
            &tm.tm_sec,
            &usec
        );

        if (r >= 6) {
            if (r == 7) {
                resource->time.tv_usec = usec;
            } else {
                resource->time.tv_usec = 0;
            }

            tm.tm_year -= 1900;
            tm.tm_mon -= 1;

            // FIXME: mktime is fundamentally broken, -1 is actually a valid time (it's the second before the Epoch), and it doesnt set errno
            t = mktime(&tm);
            if (t != -1) {
                resource->time.tv_sec = t;
                resource->dirty = 1;
                return 0;
            }
        }
    }


    // then try <seconds>.<micro_seconds>
    r = sscanf(time_str, "%d.%6d", (int *)&resource->time.tv_sec, (int *)&resource->time.tv_usec);
    if (r == 2) {
        resource->dirty = 1;
        return 0;
    }


    // finally try just <seconds>
    r = sscanf(time_str, "%d", (int *)&resource->time.tv_sec);
    resource->time.tv_usec = 0;
    if (r == 1) {
        resource->dirty = 1;
        return 0;
    }


    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_set_resource_time_from_string(): error parsing time from '%s'", time_str);
    errno = EBADMSG;
    return -1;
}


int bionet_resource_time_now(bionet_resource_t *resource) {
    int r;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_set_resource_time_from_string(): NULL resource passed in");
        errno = EINVAL;
        return -1;
    }

    r = gettimeofday(&resource->time, NULL);
    if (r < 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_set_resource_time_now(): cannot get time: %s", strerror(errno));
        return -1;
    }

    resource->dirty = 1;

    return 0;
}
#endif

