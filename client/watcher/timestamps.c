
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <glib.h>

#include "bionet-util.h"
#include "bionet.h"

const char *current_timestamp_string(void)
{
    static char time_str[200];

    char usec_str[10];
    struct tm *tm;
    int r;

    // 
    // sanity tests
    //
    struct timeval timestamp;
    r = gettimeofday(&timestamp, NULL);
    if (r != 0) {
        g_message("gettimeofday() error: %s", strerror(errno));
        exit(1);
    }


    tm = gmtime(&timestamp.tv_sec);
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

    r = snprintf(usec_str, sizeof(usec_str), ".%06ld", (long)timestamp.tv_usec);
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
