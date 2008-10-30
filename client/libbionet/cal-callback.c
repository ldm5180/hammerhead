
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include "cal-client.h"
#include "libbionet-internal.h"


void libbionet_cal_callback(const cal_event_t *event) {
    switch (event->type) {
        case CAL_EVENT_JOIN: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "Join event from '%s'\n", event->peer_name);
            break;
        }

        case CAL_EVENT_LEAVE: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "Leave event from '%s'\n", event->peer_name);
            break;
        }

        case CAL_EVENT_MESSAGE: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "Message event from '%s'\n", event->peer_name);
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "    %s\n", event->msg.buffer);  // FIXME: prolly not a NULL-terminated ASCII string...
            break;
        }

        case CAL_EVENT_PUBLISH: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "Publish event from '%s'\n", event->peer_name);
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "    %s\n", event->msg.buffer);  // FIXME: prolly not a NULL-terminated ASCII string...
            break;
        }

        default: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "unknown event type %d\n", event->type);
            break;
        }
    }
}

