
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <glib.h>

#include "libhab-internal.h"
#include "cal-server.h"
#include "bionet-asn.h"


void libhab_cal_callback(const cal_event_t *event) {
    switch (event->type) {
        case CAL_EVENT_CONNECT: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "CAL Connect event from '%s'", event->peer_name);
            break;
        }

        case CAL_EVENT_DISCONNECT: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "CAL Disconnect event from '%s'", event->peer_name);
            break;
        }

        case CAL_EVENT_MESSAGE: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "CAL Message event from '%s'", event->peer_name);
            break;
        }

        case CAL_EVENT_SUBSCRIBE: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "CAL Subscribe event from '%s' for topic '%s'", event->peer_name, event->topic);
            break;
        }

        default: {
            printf("unhandled CAL event type %d\n", event->type);
            break;
        }
    }
}

