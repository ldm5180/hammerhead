
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#define _XOPEN_SOURCE
#define _BSD_SOURCE

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <glib.h>


void str_to_timeval(const char *str, struct timeval *tv) {
    struct tm tm;
    char *p;
    char *old_tz;

    p = strptime(str, "%Y-%m-%d %T", &tm);
    if (p == NULL) {
        g_log("", G_LOG_LEVEL_ERROR, "error parsing time string '%s': %s\n", str, strerror(errno));
    }

    if (*p == '\0') {
        tv->tv_usec = 0;
    } else {
        int r;
        unsigned int val;
        int consumed;

        if (*p != '.') {
            g_log("", G_LOG_LEVEL_ERROR, "error parsing fractional seconds from time string '%s': expected decimal point after seconds\n", str);
        }
        p ++;

        if ((*p < '0') || (*p > '9')) {
            g_log("", G_LOG_LEVEL_ERROR, "error parsing fractional seconds from time string '%s': expected number after decimal point\n", str);
        }

        r = sscanf(p, "%6u%n", &val, &consumed);
        // the effect of %n on the returned conversion count is ambiguous
        if ((r != 1) && (r != 2)) {
            g_log("", G_LOG_LEVEL_ERROR, "error parsing fractional seconds from time string '%s': didn't find \"%%u\"\n", str);
        }
        if (consumed != strlen(p)) {
            g_log("", G_LOG_LEVEL_ERROR, "error parsing fractional seconds from time string '%s': garbage at end\n", str);
        }
        if (consumed > 6) {
            g_log("", G_LOG_LEVEL_ERROR, "error parsing fractional seconds from time string '%s': number too long\n", str);
        }

        tv->tv_usec = val * pow(10, 6-consumed);
    }

    old_tz = getenv("TZ");
    setenv("TZ", "UTC", 1);
    tv->tv_sec = mktime(&tm);
    if (old_tz) {
        char sanitized[128];
        snprintf(sanitized, 128, "%s", old_tz);
        setenv("TZ", sanitized, 1);
    } else {
        unsetenv("TZ");
    }
}

