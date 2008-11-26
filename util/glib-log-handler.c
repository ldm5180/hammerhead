
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdio.h>

#if defined(LINUX) || defined(MACOSX)
    #include <syslog.h>
#endif

#include <glib.h>

#include "bionet-util.h"


void bionet_glib_log_handler(
    const gchar *log_domain,
    GLogLevelFlags log_level,
    const gchar *message,
    gpointer log_context
) {
    static bionet_log_context_t default_log_context = {
        destination: BIONET_LOG_TO_STDOUT,
        log_limit: G_LOG_LEVEL_INFO
    };

    bionet_log_context_t *lc;


    if (log_context == NULL) {
        lc = &default_log_context;
    } else {
        lc = log_context;
    }


    if (log_level > lc->log_limit) return;

    switch (lc->destination) {
        case BIONET_LOG_TO_SYSLOG: {
#if defined(LINUX) || defined(MACOSX)
            if ((log_domain == NULL) || (log_domain[0] == '\0')) {
                syslog(LOG_INFO, "%s", message);
            } else {
                syslog(LOG_INFO, "%s: %s\n", log_domain, message);
            }
#endif
            return;
        }

        case BIONET_LOG_TO_STDOUT: {
            if ((log_domain == NULL) || (log_domain[0] == '\0')) {
                printf("%s\n", message);
            } else {
                printf("%s: %s\n", log_domain, message);
            }
            fflush(stdout);
            return;
        }

        default: {
            return;
        }
    }
}

