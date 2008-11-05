
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include "cal-client.h"
#include "libbionet-internal.h"
#include "bionet-asn.h"




static void handle_server_message(const cal_event_t *event) {
    H2C_Message_t *m = NULL;
    asn_dec_rval_t rval;

    rval = ber_decode(NULL, &asn_DEF_H2C_Message, (void **)&m, event->msg.buffer, event->msg.size);
    if (rval.code == RC_WMORE) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "server message from '%s' contained an incomplete ASN.1 message", event->peer_name);
        return;
    } else if (rval.code == RC_FAIL) {
        // received invalid junk
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "server message from '%s' contained an invalid ASN.1 message", event->peer_name);
        return;
    } else if (rval.code != RC_OK) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "unknown error (code=%d) decoding server message from '%s'", rval.code, event->peer_name);
        return;
    }

    if (rval.consumed != event->msg.size) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "server message from '%s' contained junk at end of message (consumed %d of %d)", event->peer_name, (int)rval.consumed, event->msg.size);
    }


    switch (m->present) {

        case H2C_Message_PR_newNode: {
            char *hab_type;
            char *hab_id;

            bionet_node_t *node;
            bionet_hab_t *hab;

            int r;

            r = bionet_split_hab_name(event->peer_name, &hab_type, &hab_id);
            if (r != 0) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error parsing HAB name from CAL peer name '%s'", event->peer_name);
                break;
            }

            hab = bionet_cache_lookup_hab(hab_type, hab_id);
            if (hab == NULL) {
                hab = bionet_hab_new(hab_type, hab_id);
                if (hab == NULL) {
                    // an error has been logged already
                    break;
                }
                libbionet_cache_add_hab(hab);
            }

            node = bionet_asn_to_node(&m->choice.newNode);
            if (node == NULL) {
                // an error has been logged
                break;
            }
            node->hab = hab;

            bionet_hab_add_node(hab, node);

            if (libbionet_callback_new_node != NULL) {
                libbionet_callback_new_node(node);
            }

            break;
        }


        case H2C_Message_PR_lostNode: {
            char *hab_type;
            char *hab_id;

            bionet_hab_t *hab;
            bionet_node_t *node;

            int r;

            r = bionet_split_hab_name(event->peer_name, &hab_type, &hab_id);
            if (r != 0) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error parsing HAB name from CAL peer name '%s'", event->peer_name);
                break;
            }

            hab = bionet_cache_lookup_hab(hab_type, hab_id);
            if (hab == NULL) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "got lost-node message for Node '%s.%s', but that HAB doesn appear in the cache", event->peer_name, (char*)m->choice.lostNode.buf);
                break;
            }

            node = bionet_cache_lookup_node(hab_type, hab_id, (const char *)m->choice.lostNode.buf);
            if (node == NULL) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "got lost-node message for Node '%s.%s', which doesnt appear in the cache", event->peer_name, (char*)m->choice.lostNode.buf);
                break;
            }

            if (libbionet_callback_lost_node != NULL) {
                libbionet_callback_lost_node(node);
            }

            bionet_hab_remove_node_by_id(hab, node->id);
            bionet_node_free(node);

            break;
        }


        default: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "dont know what to do with H2C message type %d from %s", m->present, event->peer_name);
            xer_fprint(stdout, &asn_DEF_H2C_Message, m);
            break;
        }

    }

    asn_DEF_H2C_Message.free_struct(&asn_DEF_H2C_Message, m, 0);
}




void libbionet_cal_callback(const cal_event_t *event) {
    switch (event->type) {
        case CAL_EVENT_JOIN: {
            bionet_hab_t *hab;
            char *type;
            char *id;
            int r;

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

            // if the client's been told about this HAB or any of its nodes, report them as lost now
            hab = bionet_cache_lookup_hab(type, id);
            if (hab != NULL) {
                if (libbionet_callback_lost_node != NULL) {
                    GSList *i;
                    for (i = hab->nodes; i != NULL; i = i->next) {
                        bionet_node_t *node = i->data;
                        libbionet_callback_lost_node(node);
                    }
                }

                bionet_hab_remove_all_nodes(hab);

                if (libbionet_callback_lost_hab != NULL) {
                    libbionet_callback_lost_hab(hab);
                }
                libbionet_cache_remove_hab(hab);
            }

            break;
        }

        case CAL_EVENT_MESSAGE: {
            handle_server_message(event);
            break;
        }

        case CAL_EVENT_PUBLISH: {
            handle_server_message(event);
            break;
        }

        default: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "CAL unknown event type %d", event->type);
            break;
        }
    }
}

