
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

            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "CAL Join event from '%s'", event->peer_name);

            r = bionet_split_hab_name(event->peer_name, &type, &id);
            if (r != 0) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "CAL peer name '%s' is not a valid Bionet HAB name, ignoring", event->peer_name);
                break;
            }

            hab = bionet_hab_new(type, id);

            // add the hab to the bionet library's list of known habs
            libbionet_habs = g_slist_prepend(libbionet_habs, hab);

            // see if we need to publish this to the user
            {
                GSList *i;

                for (i = libbionet_hab_subscriptions; i != NULL; i = i->next) {
                    libbionet_hab_subscription_t *sub = i->data;

                    if (bionet_hab_matches_type_and_id(hab, sub->hab_type, sub->hab_id)) {
                        libbionet_cache_add_hab(hab);
                        if (libbionet_callback_new_hab != NULL) {
                            libbionet_callback_new_hab(hab);
                        }
                        break;
                    }
                }
            }

            break;
        }

        case CAL_EVENT_LEAVE: {
            bionet_hab_t *hab;
            char *type;
            char *id;
            int r;

            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "CAL Leave event from '%s'", event->peer_name);

            r = bionet_split_hab_name(event->peer_name, &type, &id);
            if (r != 0) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "CAL peer name '%s' is not a valid Bionet HAB name, ignoring", event->peer_name);
                break;
            }

            // remove this hab from the internal list of known habs
            {
                GSList *i;

                for (i = libbionet_habs; i != NULL; i = i->next) {
                    bionet_hab_t *hab = i->data;

                    if (bionet_hab_matches_type_and_id(hab, type, id)) {
                        libbionet_habs = g_slist_remove(libbionet_habs, hab);
                        break;
                    }
                }
            }

            hab = bionet_cache_lookup_hab(type, id);
            if (hab != NULL) {
                if (libbionet_callback_lost_hab != NULL) {
                    libbionet_callback_lost_hab(hab);
                }
                libbionet_cache_remove_hab(hab);
            }

            break;
        }

        case CAL_EVENT_MESSAGE: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "CAL Message event from '%s'", event->peer_name);
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "    %s", event->msg.buffer);  // FIXME: prolly not a NULL-terminated ASCII string...
            break;
        }

        case CAL_EVENT_PUBLISH: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "CAL Publish event from '%s'", event->peer_name);
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "    %s", event->msg.buffer);  // FIXME: prolly not a NULL-terminated ASCII string...
            break;
        }

        default: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "CAL unknown event type %d", event->type);
            break;
        }
    }
}

