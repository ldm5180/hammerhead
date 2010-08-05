
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <glib.h>

char * timeval_as_str(
    struct timeval *tv,
    char * time_str, size_t size)
{
    char usec_str[10];
    struct tm *tm;
    struct timeval local_tv;
    int r;

    if(tv == NULL) {
        r = gettimeofday(&local_tv, NULL);
        if(r) {
            g_message("%s(): Failed to get current time: %s",
                    __FUNCTION__, strerror(errno));
            goto error;
        }

        tv = &local_tv;
    }

    tm = gmtime((time_t *)&tv->tv_sec);
    if (tm == NULL) {
        g_message("%s(): error with gmtime: %s", __FUNCTION__,
	      strerror(errno));
        goto error;
    }

    r = strftime(time_str, size, "%Y-%m-%d %H:%M:%S", tm);
    if (r <= 0) {
        g_message("%s(): error with strftime: %s", __FUNCTION__,
	      strerror(errno));
        goto error;
    }

    r = snprintf(usec_str, sizeof(usec_str), ".%06ld", 
            (long)tv->tv_usec);
    if (r >= sizeof(usec_str)) {
        // this should never happen, but it keeps Coverity happy
        g_message("%s(): usec_str too small?!", __FUNCTION__);
        goto error;
    }

    // sanity check destination memory size available
    if ((strlen(usec_str) + 1 + strlen(time_str)) > size) {
        g_message("%s(): time_str too small?!", __FUNCTION__);
        goto error;
    }
    strncat(time_str, usec_str, strlen(usec_str));

    return time_str;

error:
    strncpy(time_str, "invalid time", size);
    time_str[size-1] = '\0';
    return time_str;
}
