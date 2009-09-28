
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "bionet-util.h"
#include "cal-client.h"
#include "libbionet-internal.h"
#include "bionet-asn.h"





static void handle_new_node(const cal_event_t *event, const Node_t *newNode) {
    char *hab_type;
    char *hab_id;

    bionet_node_t *node;
    bionet_hab_t *hab;

    int r;

    r = bionet_split_hab_name(event->peer_name, &hab_type, &hab_id);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error parsing HAB name from CAL peer name '%s'", event->peer_name);
        return;
    }

    hab = bionet_cache_lookup_hab(hab_type, hab_id);
    if (hab == NULL) {
        hab = bionet_hab_new(hab_type, hab_id);
        if (hab == NULL) {
            // an error has been logged already
            return;
        }
        libbionet_cache_add_hab(hab);
    }

    node = bionet_asn_to_node_21(newNode, hab);
    if (node == NULL) {
        // an error has been logged
        return;
    }

    // don't log an error message (the hab may already be in the cache)
    bionet_hab_add_node(hab, node);

    if (libbionet_callback_new_node != NULL) {
        libbionet_callback_new_node(node);
    }
}




// FIXME: these should go in the internal cache
static void handle_resource_metadata(const cal_event_t *event, const ResourceMetadata_t *rm) {
    char *hab_type;
    char *hab_id;

    bionet_hab_t *hab;
    bionet_node_t *node;
    bionet_resource_t *resource;

    int r;

    r = bionet_split_hab_name(event->peer_name, &hab_type, &hab_id);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error parsing HAB name from CAL peer name '%s'", event->peer_name);
        return;
    }

    hab = bionet_cache_lookup_hab(hab_type, hab_id);
    if (hab == NULL) {
        hab = bionet_hab_new(hab_type, hab_id);
        if (hab == NULL) {
            // an error has been logged already
            return;
        }
        libbionet_cache_add_hab(hab);
    }

    node = bionet_cache_lookup_node(
        hab_type,
        hab_id,
        (const char *)rm->nodeId.buf
    );
    if (node == NULL) {
        node = bionet_node_new(hab, (const char *)rm->nodeId.buf);
        if (node == NULL) {
            // an error has been logged already
            return;
        }
        libbionet_cache_add_node(node);
    }

    resource = bionet_cache_lookup_resource(
        hab_type,
        hab_id,
        (const char *)rm->nodeId.buf,
        (const char *)rm->resourceId.buf
    );
    if (resource == NULL) {
        bionet_resource_data_type_t data_type;
        bionet_resource_flavor_t flavor;

        data_type = bionet_asn_to_datatype(rm->datatype);
        if (data_type == BIONET_RESOURCE_DATA_TYPE_INVALID) {
            // an error has been logged already;
            return;
        }

        flavor = bionet_asn_to_flavor(rm->flavor);
        if (flavor == BIONET_RESOURCE_FLAVOR_INVALID) {
            // an error has been logged already;
            return;
        }

        resource = bionet_resource_new(
            node,
            data_type,
            flavor,
            (const char *)rm->resourceId.buf
        );
        if (resource == NULL) {
            // an error has been logged already
            return;
        }
        libbionet_cache_add_resource(resource);
    }
}






static void handle_lost_node(const cal_event_t *event, const PrintableString_t *nodeId) {
    char *hab_type;
    char *hab_id;

    bionet_hab_t *hab;
    bionet_node_t *node;

    int r;

    r = bionet_split_hab_name(event->peer_name, &hab_type, &hab_id);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error parsing HAB name from CAL peer name '%s'", event->peer_name);
        return;
    }

    hab = bionet_cache_lookup_hab(hab_type, hab_id);
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "got lost-node message for Node '%s.%s', but that HAB doesnt appear in the cache", event->peer_name, (char*)nodeId->buf);
        return;
    }

    node = bionet_cache_lookup_node(hab_type, hab_id, (const char *)nodeId->buf);
    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "got lost-node message for Node '%s.%s', which doesnt appear in the cache", event->peer_name, (char*)nodeId->buf);
        return;
    }

    if (libbionet_callback_lost_node != NULL) {
        libbionet_callback_lost_node(node);
    }

    bionet_hab_remove_node_by_id(hab, bionet_node_get_id(node));
    bionet_node_free(node);
}




static void handle_resource_datapoints(const cal_event_t *event, ResourceDatapoints_t *rd) {
    char *hab_type;
    char *hab_id;

    bionet_resource_t *resource;

    int i;

    int r;


    r = bionet_split_hab_name(event->peer_name, &hab_type, &hab_id);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error parsing HAB name from CAL peer name '%s'", event->peer_name);
        return;
    }

    resource = bionet_cache_lookup_resource(
        hab_type,
        hab_id,
        (const char *)rd->nodeId.buf,
        (const char *)rd->resourceId.buf
    );
    if (resource == NULL) {
        g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "got a Datapoint for unknown Resource %s.%s.%s:%s",
            hab_type,
            hab_id,
            (const char *)rd->nodeId.buf,
            (const char *)rd->resourceId.buf
        );
        return;
    }

    for (i = 0; i < rd->newDatapoints.list.count; i ++) {
        Datapoint_t *asn_d = rd->newDatapoints.list.array[i];
        bionet_datapoint_t *new_d = bionet_asn_to_datapoint(asn_d, resource);

        if( NULL == new_d ){
            // An error has been logged already
            continue;
        }

        bionet_resource_remove_datapoint_by_index(resource, 0);
        bionet_resource_add_datapoint(resource, new_d);

        if (libbionet_callback_datapoint != NULL) {
            libbionet_callback_datapoint(new_d);
        }
    }
}




static void handle_stream_data(const cal_event_t *event, StreamData_t *sd) {
    char *hab_type;
    char *hab_id;

    bionet_stream_t *stream;

    int r;


    r = bionet_split_hab_name(event->peer_name, &hab_type, &hab_id);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error parsing HAB name from CAL peer name '%s'", event->peer_name);
        return;
    }

    stream = bionet_cache_lookup_stream(
        hab_type,
        hab_id,
        (const char *)sd->nodeId.buf,
        (const char *)sd->streamId.buf
    );
    if (stream == NULL) {
        g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "got Stream data for unknown Stream %s.%s.%s:%s",
            hab_type,
            hab_id,
            (const char *)sd->nodeId.buf,
            (const char *)sd->streamId.buf
        );
        return;
    }

    if (libbionet_callback_stream!= NULL) {
        libbionet_callback_stream(stream, sd->data.buf, sd->data.size);
    }
}




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
            handle_new_node(event, &m->choice.newNode);
            break;
        }

        case H2C_Message_PR_lostNode: {
            handle_lost_node(event, &m->choice.lostNode);
            break;
        }

        case H2C_Message_PR_resourceMetadata: {
            handle_resource_metadata(event, &m->choice.resourceMetadata);
            break;
        }

        case H2C_Message_PR_datapointsUpdate: {
            handle_resource_datapoints(event, &m->choice.datapointsUpdate);
            break;
        }

        case H2C_Message_PR_streamData: {
            handle_stream_data(event, &m->choice.streamData);
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
	    if (NULL == hab) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		      "Failed to get a new HAB.");
		return;
	    }

	    bionet_hab_set_secure(hab, event->is_secure);

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
            bionet_hab_t *hab = NULL;
            char *type;
            char *id;
            int r;

            r = bionet_split_hab_name(event->peer_name, &type, &id);
            if (r != 0) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "CAL peer name '%s' is not a valid Bionet HAB name, ignoring", event->peer_name);
                break;
            }


            //
            // it's a bit confusing because there are two lists of HABs:
            // the list of HABs that the client has been told about, and
            // the "master" internal list of everything the process has seen
            //


            // if the client's been told about this HAB or any of its nodes, report them as lost now
            hab = bionet_cache_lookup_hab(type, id);
            if (hab != NULL) {
                GSList *j;

                if (libbionet_callback_lost_node != NULL) {
                    int i;
                    for (i = 0; i < bionet_hab_get_num_nodes(hab); i++) {
                        bionet_node_t *node = bionet_hab_get_node_by_index(hab, i);
                        GSList *j;
                        int found_node_subscription = 0;
                        
                        // only report the node if we are subscribed to it!
                        for (j = libbionet_node_subscriptions; j != NULL; j = j->next) {
                            libbionet_node_subscription_t *node_sub = j->data;

                            if (bionet_node_matches_habtype_habid_nodeid(node, 
                                node_sub->hab_type, node_sub->hab_id, node_sub->node_id)) {
                                found_node_subscription = 1;
                                break;
                            }
                        }
                        
                        if (!found_node_subscription)
                            continue;
                        
                        libbionet_callback_lost_node(node);
                    }
                }

                bionet_hab_remove_all_nodes(hab);

                if (libbionet_callback_lost_hab != NULL) {
                    for (j = libbionet_hab_subscriptions; j != NULL; j = j->next) {
                        libbionet_hab_subscription_t *sub = j->data;

                        if (bionet_hab_matches_type_and_id(hab, sub->hab_type, sub->hab_id)) {
                            libbionet_callback_lost_hab(hab);
                            break;
                        }
                    }
                }

                libbionet_cache_remove_hab(hab);
            }

            // remove the hab from the "master" internal list of habs and free it
            {
                GSList *i;

                for (i = libbionet_habs; i != NULL; i = i->next) {
                    bionet_hab_t *hab = i->data;

                    if (bionet_hab_matches_type_and_id(hab, type, id)) {
                        libbionet_habs = g_slist_remove(libbionet_habs, hab);
                        bionet_hab_free(hab);
                        break;
                    }
                }
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


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
