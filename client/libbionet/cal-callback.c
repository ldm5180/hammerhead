
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include "cal-client.h"
#include "libbionet-internal.h"


void libbionet_cal_callback(const cal_event_t *event) {
    switch (event->type) {
        case CAL_EVENT_JOIN: {
            bionet_hab_t *hab;
            char *type;
            char *id;
            int r;

            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "CAL Join event from '%s'\n", event->peer_name);

            r = bionet_split_hab_name(event->peer_name, &type, &id);
            if (r != 0) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "CAL peer name '%s' is not a valid Bionet HAB name, ignoring\n", event->peer_name);
                break;
            }

            hab = bionet_hab_new(type, id);
            libbionet_cache_add_hab(hab);

            if (libbionet_callback_new_hab != NULL) {
                libbionet_callback_new_hab(hab);
            }

            break;
        }

        case CAL_EVENT_LEAVE: {
            bionet_hab_t *hab;
            char *type;
            char *id;
            int r;

            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "CAL Leave event from '%s'\n", event->peer_name);

            r = bionet_split_hab_name(event->peer_name, &type, &id);
            if (r != 0) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "CAL peer name '%s' is not a valid Bionet HAB name, ignoring\n", event->peer_name);
                break;
            }

            hab = bionet_cache_lookup_hab(type, id);
            if (hab == NULL) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "CAL Leave event from unknown HAB '%s'\n", event->peer_name);
                break;
            }

            if (libbionet_callback_lost_hab != NULL) {
                libbionet_callback_lost_hab(hab);
            }

            libbionet_cache_remove_hab(hab);

            break;
        }

        case CAL_EVENT_MESSAGE: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "CAL Message event from '%s'\n", event->peer_name);
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "    %s\n", event->msg.buffer);  // FIXME: prolly not a NULL-terminated ASCII string...
            break;
        }

        case CAL_EVENT_PUBLISH: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "CAL Publish event from '%s'\n", event->peer_name);
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "    %s\n", event->msg.buffer);  // FIXME: prolly not a NULL-terminated ASCII string...
            break;
        }

        default: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "CAL unknown event type %d\n", event->type);
            break;
        }
    }
}

