
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <errno.h>

#include "internal.h"
#include "bionet-util.h"
#include "cal-client.h"
#include "libbdm-internal.h"

#include "bionet-asn.h"

#if 0
// We synthesize new/lost hab and node mesages until those events are tracked by the server
static void bdm_handle_new_node(const cal_event_t *event, const BDMNewNode_t *newNode) {
    bionet_node_t *node;
    bionet_hab_t *hab;
    bionet_bdm_t *bdm;

    int crossLink = 0;

    bdm = bdm_cache_lookup_bdm((const char *)newNode->bdmId.buf);
    if (bdm == NULL) {
        bdm = bionet_bdm_new((const char*)newNode->bdmId.buf);
        if (bdm == NULL) {
            // an error has been logged already
            return;
        }
        libbdm_cache_add_bdm(bdm);
        crossLink = 1;
    }

    hab = bdm_cache_lookup_hab((const char*)newNode->habType.buf, (const char*)newNode->habId.buf);
    if (hab == NULL) {
        hab = bionet_hab_new((const char*)newNode->habType.buf, (const char*)newNode->habId.buf);
        if (hab == NULL) {
            // an error has been logged already
            return;
        }
        libbdm_cache_add_hab(hab);
        crossLink = 1;

    }

    if(crossLink) {
        bionet_hab_add_bdm(hab, bdm);
        bionet_bdm_add_hab(bdm, hab);
    }


    node = bionet_asn_to_node_21(&newNode->node, hab);
    if (node == NULL) {
        // an error has been logged
        return;
    }

    // don't log an error message (the hab may already be in the cache)
    bionet_hab_add_node(hab, node);

    if (libbdm_callback_new_node != NULL) {
        libbdm_callback_new_node(node, libbdm_callback_new_node_usr_data);
    }
}
#endif

static void bdm_handle_resource_metadata(const cal_event_t *event, const BDMResourceMetadata_t *rm) {
    bionet_bdm_t *bdm;
    bionet_hab_t *hab;
    bionet_node_t *node;
    bionet_resource_t *resource;

    const char * hab_type = (const char*)rm->habType.buf;
    const char * hab_id = (const char*)rm->habId.buf;
    const char * node_id = (const char*)rm->nodeId.buf;
    const char * resource_id = (const char*)rm->resourceId.buf;

    bionet_bdm_t * bdm_state = g_hash_table_lookup(libbdm_all_peers, event->peer_name);
    if (NULL == bdm_state) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "message from unknown peer %s", event->peer_name);
    }

    bdm_state->curr_seq = rm->entrySeq;


    bdm = bdm_cache_lookup_bdm(event->peer_name);
    if (bdm == NULL) {
        bdm = bionet_bdm_new(event->peer_name);
        if (bdm == NULL) {
            // an error has been logged already
            return;
        }
        libbdm_cache_add_bdm(bdm);
    }

    hab = bdm_cache_lookup_hab(hab_type, hab_id);
    if (hab == NULL) {
        hab = bionet_hab_new(hab_type, hab_id);
        if (hab == NULL) {
            // an error has been logged already
            return;
        }
        libbdm_cache_add_hab(hab);

        bionet_hab_add_bdm(hab, bdm);
        bionet_bdm_add_hab(bdm, hab);

        // see if we need to publish this to the user
        {
            GSList *i;

            for (i = libbdm_hab_subscriptions; i != NULL; i = i->next) {
                libbdm_hab_subscription_t *sub = i->data;

                if (bionet_hab_matches_type_and_id(hab, sub->hab_type, sub->hab_id)) {
                    if (libbdm_callback_new_hab != NULL) {
                        libbdm_callback_new_hab(hab, libbdm_callback_new_hab_usr_data);
                    }
                    break;
                }
            }
        }
    }

    node = bionet_hab_get_node_by_id(hab, node_id);
    if (node == NULL) {
        node = bionet_node_new(hab, node_id);
        if (node == NULL) {
            // an error has been logged already
            return;
        }
        libbdm_cache_add_node(node);

        // see if we need to publish this to the user
        {
            GSList *i;

            for (i = libbdm_node_subscriptions; i != NULL; i = i->next) {
                libbdm_node_subscription_t *sub = i->data;

                if (bionet_node_matches_id(node, sub->node_id)) {
                    if (libbdm_callback_new_node != NULL) {
                        libbdm_callback_new_node(node, libbdm_callback_new_node_usr_data);
                    }
                    break;
                }
            }
        }
    }

    resource = bdm_cache_lookup_resource(
        hab_type, hab_id, node_id, resource_id
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
            resource_id
        );
        if (resource == NULL) {
            // an error has been logged already
            return;
        }
        libbdm_cache_add_resource(resource);
    }
}

static void bdm_handle_resource_datapoints(const cal_event_t *event, BDMResourceDatapoints_t *rd) {
    bionet_resource_t *resource;
    int i;

    const char * hab_type = (const char*)rd->habType.buf;
    const char * hab_id = (const char*)rd->habId.buf;
    const char * node_id = (const char*)rd->nodeId.buf;
    const char * resource_id = (const char*)rd->resourceId.buf;

    bionet_bdm_t * bdm_state = g_hash_table_lookup(libbdm_all_peers, event->peer_name);
    if (NULL == bdm_state) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "message from unknown peer %s", event->peer_name);
    }

    bdm_state->curr_seq = rd->entrySeq;

    resource = bdm_cache_lookup_resource(hab_type, hab_id, node_id, resource_id);
    if (resource == NULL) {
        g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "got a Datapoint for unknown Resource %s.%s.%s:%s",
            hab_type, hab_id, node_id, resource_id
        );
        return;
    }

    for (i = 0; i < rd->newDatapointsBDM.list.count; i ++) {
        Datapoint_t *asn_d = rd->newDatapointsBDM.list.array[i];
        bionet_datapoint_t *new_d = bionet_asn_to_datapoint(asn_d, resource);

        if( NULL == new_d ){
            // An error has been logged already
            continue;
        }

        bionet_resource_remove_datapoint_by_index(resource, 0);
        bionet_resource_add_datapoint(resource, new_d);

        if (libbdm_callback_datapoint != NULL) {
            libbdm_callback_datapoint(new_d, libbdm_callback_datapoint_usr_data);
        }
    }
}


#if 0
// TODO: Handle streams over BDM
static void bdm_handle_stream_data(const cal_event_t *event, StreamData_t *sd) {
    bionet_stream_t *stream;

    int r;


    stream = bdm_cache_lookup_stream(
        rm->habType.buf,
        rm->habId.buf,
        (const char *)sd->nodeId.buf,
        (const char *)sd->streamId.buf
    );
    if (stream == NULL) {
        g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "got Stream data for unknown Stream %s.%s.%s:%s",
            rm->habType.buf,
            rm->habId.buf,
            (const char *)sd->nodeId.buf,
            (const char *)sd->streamId.buf
        );
        return;
    }

    if (libbdm_callback_stream!= NULL) {
        libbdm_callback_stream(stream, sd->data.buf, sd->data.size,
                libbdm_callback_stream_usr_data);
    }
}
#endif

// Handle a new subscription by sending state to server
// TODO: Get the state from a user-specified persistant location
static void bdm_handle_server_subscribe(const cal_event_t *event) {

    const char * peer_name = event->peer_name;
    const char * topic = event->topic;

    int r;
    BDM_C2S_Message_t m;
    bionet_asn_buffer_t buf;
    asn_enc_rval_t enc_rval;

    bionet_bdm_t * bdm_state = g_hash_table_lookup(libbdm_all_peers, peer_name);
    if (NULL == bdm_state) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "message from unknown peer %s", event->peer_name);
    }

    memset(&m, 0x00, sizeof(BDM_C2S_Message_t));
    memset(&buf, 0, sizeof(buf));

    m.present = BDM_C2S_Message_PR_sendState;
    m.choice.sendState.seq = bdm_state->curr_seq;

    r = OCTET_STRING_fromBuf(&m.choice.sendState.topic, topic, -1);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set topic", __FUNCTION__);
        goto cleanup;

    }

    enc_rval = der_encode(&asn_DEF_BDM_C2S_Message, &m, bionet_accumulate_asn_buffer, &buf);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_C2S_Message, &m);
    if (enc_rval.encoded == -1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_get_resource_datapoints(): error with der_encode(): %s", strerror(errno));
        goto cleanup;
    }

    // send the state to the BDM
    // NOTE: cal_client.sendto assumes controll of buf
    r = cal_client.sendto(peer_name, buf.buf, buf.size);

    return;

cleanup:
    free(buf.buf);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_C2S_Message, &m);
}

static void bdm_handle_server_state_update(const cal_event_t *event, BDMSendState_t *state) {

    bionet_bdm_t * bdm_state = g_hash_table_lookup(libbdm_all_peers, event->peer_name);
    if (NULL == bdm_state) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "message from unknown peer %s", event->peer_name);
    }
    bdm_state->curr_seq = state->seq;
}

static void bdm_handle_server_message(const cal_event_t *event) {
    BDM_S2C_Message_t *m = NULL;
    asn_dec_rval_t rval;

    rval = ber_decode(NULL, &asn_DEF_BDM_S2C_Message, (void **)&m, event->msg.buffer, event->msg.size);
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

        case BDM_S2C_Message_PR_newHab: {
            //bdm_handle_new_hab(event, &m->choice.newHab);
            break;
        }

        case BDM_S2C_Message_PR_lostHab: {
            //bdm_handle_lost_hab(event, &m->choice.lostHab);
            break;
        }

        case BDM_S2C_Message_PR_newNode: {
            //bdm_handle_new_node(event, &m->choice.newNode);
            break;
        }

        case BDM_S2C_Message_PR_lostNode: {
            //bdm_handle_lost_node(event, &m->choice.lostNode);
            break;
        }

        case BDM_S2C_Message_PR_resourceMetadata: {
            bdm_handle_resource_metadata(event, &m->choice.resourceMetadata);
            break;
        }

        case BDM_S2C_Message_PR_datapointsUpdate: {
            bdm_handle_resource_datapoints(event, &m->choice.datapointsUpdate);
            break;
        }

        /* TODO: Handle Stream Data
        case BDM_S2C_Message_PR_streamData: {
            bdm_handle_stream_data(event, &m->choice.streamData);
            break;
        }
        */

        case BDM_S2C_Message_PR_resourceDatapointsReply: {
            bdm_handle_query_response(event, &m->choice.resourceDatapointsReply);
            break;
        }

        case BDM_S2C_Message_PR_sendState: {
            bdm_handle_server_state_update(event, &m->choice.sendState);
            break;
        }

        default: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "dont know what to do with BDM_S2C message type %d from %s", m->present, event->peer_name);
            xer_fprint(stdout, &asn_DEF_BDM_S2C_Message, m);
            break;
        }

    }

    asn_DEF_BDM_S2C_Message.free_struct(&asn_DEF_BDM_S2C_Message, m, 0);
}


void libbdm_cal_callback(const cal_event_t *event) {
    switch (event->type) {
        case CAL_EVENT_JOIN: {
            bionet_bdm_t *bdm;

            // add the bdm to the bdm library's list of known bdms
            if ( NULL == (bdm = g_hash_table_lookup( libbdm_all_peers, event->peer_name)) ) {

                bdm = bionet_bdm_new(event->peer_name);
                if (NULL == bdm) {
                    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                          "Failed to get a new BDM.");
                    return;
                }
                g_hash_table_insert(libbdm_all_peers, strdup(event->peer_name), bdm);
            }

            // TODO: Implement BDM cal-security
	    //bionet_bdm_set_secure(bdm, event->is_secure);


            // see if we need to publish this to the BDM API
            {
                GSList *i;

                for (i = libbdm_bdm_api_subscriptions; i != NULL; i = i->next) {
                    libbdm_bdm_subscription_t *sub = i->data;

                    if (bionet_name_component_matches(event->peer_name, sub->bdm_id)) {
                        char * newbdm = strdup(event->peer_name);
                        libbdm_api_new_peers = g_slist_prepend(libbdm_api_new_peers, newbdm);
                        break;
                    }
                }
            }


            // see if we need to publish this to the user
            {
                GSList *i;

                for (i = libbdm_bdm_subscriptions; i != NULL; i = i->next) {
                    libbdm_bdm_subscription_t *sub = i->data;

                    if (bionet_name_component_matches(bdm->id, sub->bdm_id)) {
                        libbdm_cache_add_bdm(bdm);
                        if (libbdm_callback_new_bdm != NULL) {
                            libbdm_callback_new_bdm(bdm, libbdm_callback_new_bdm_usr_data);
                        }
                        break;
                    }
                }
            }

            break;
        }

        case CAL_EVENT_LEAVE: {
            bionet_bdm_t *bdm = NULL;

            {
                GSList *i;

                for (i = libbdm_bdm_api_subscriptions; i != NULL; i = i->next) {
                    libbdm_bdm_subscription_t *sub = i->data;

                    if (bionet_name_component_matches(event->peer_name, sub->bdm_id)) {
                        char * lostbdm = strdup(event->peer_name);
                        libbdm_api_lost_peers = g_slist_prepend(libbdm_api_lost_peers, lostbdm);
                        break;
                    }
                }
            }
            //
            // it's a bit confusing because there are two lists of BDMs:
            // the list of BDMs that the client has been told about (the cache), and
            // the "master" internal list of every BDM the process has discovered 
            // (libbdm_all_peers)
            //


            bdm = bdm_cache_lookup_bdm(event->peer_name);
            if (bdm != NULL) {
                libbdm_cache_remove_bdm(bdm);
            }

            // keep the bdm in libbdm_all_peers, so that the state is maintained if the peer returns

            break;
        }

        case CAL_EVENT_MESSAGE: {
            bdm_handle_server_message(event);
            break;
        }

        case CAL_EVENT_PUBLISH: {
            bdm_handle_server_message(event);
            break;
        }

        case CAL_EVENT_SUBSCRIBE: {
            bdm_handle_server_subscribe(event);
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
