
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include "cal-client.h"
#include "libbionet-internal.h"
#include "bionet-asn.h"


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
            H2C_Message_t *m = NULL;
            asn_dec_rval_t rval;

            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "CAL Publish event from '%s'", event->peer_name);

            rval = ber_decode(NULL, &asn_DEF_H2C_Message, (void **)&m, event->msg.buffer, event->msg.size);
            if (rval.code == RC_WMORE) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "CAL Publish event from '%s' contained an incomplete ASN.1 message", event->peer_name);
                break;
            } else if (rval.code == RC_FAIL) {
                // received invalid junk
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "CAL Publish event from '%s' contained an invalid ASN.1 message", event->peer_name);
                break;
            } else if (rval.code != RC_OK) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "unknown error (code=%d) decoding CAL Publish event from '%s'", rval.code, event->peer_name);
                break;
            }

            if (rval.consumed != event->msg.size) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "CAL Publish event from '%s' contained junk at end of message, dropping (consumed %d of %d)", event->peer_name, (int)rval.consumed, event->msg.size);
                // break;
            }

            xer_fprint(stdout, &asn_DEF_H2C_Message, m);

            asn_DEF_H2C_Message.free_struct(&asn_DEF_H2C_Message, m, 0);

#if 0
            switch (m->present) {
                case H2C_Message_PR_newNode: {
                    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "    new-node:", event->msg.buffer);  // FIXME: prolly not a NULL-terminated ASCII string...
#endif

            break;
        }

        default: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "CAL unknown event type %d", event->type);
            break;
        }
    }
}

