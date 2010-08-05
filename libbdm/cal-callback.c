
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <errno.h>

#include "internal.h"
#include "bionet-util.h"
#include "cal-client.h"
#include "libbdm-internal.h"

#include "bionet-asn.h"
#include "bdm-asn.h"
#include "../util/protected.h"

static void bdm_handle_new_hab(const cal_event_t *event, const BDMNewHab_t *newHab) {
    bionet_hab_t *hab;
    bionet_event_t * bionet_event;
    int i;

    hab = bdm_cache_lookup_hab((const char*)newHab->habType.buf, (const char*)newHab->habId.buf);
    if (hab == NULL) {
        hab = bionet_hab_new((const char*)newHab->habType.buf, (const char*)newHab->habId.buf);
        if (hab == NULL) {
            // an error has been logged already
            return;
        }

        libbdm_cache_add_hab(hab);
    }

    for(i=0; i<bionet_hab_get_num_nodes(hab); i++) {
        bionet_node_t *new_node, *old_node;
        const uint8_t * node_uid;


        new_node = bionet_hab_get_node_by_index(hab, i);
        node_uid = bionet_node_get_uid(new_node);
        old_node = bdm_cache_lookup_node_uid(node_uid);

        if(NULL == old_node) {
            libbdm_cache_add_node(new_node);
        } else if ( old_node != new_node) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
                "%s(): New node already exists in cache %s " UUID_FMTSTR , 
                __FUNCTION__, bionet_node_get_name(old_node), UUID_ARGS(node_uid));
        }
    }

    struct timeval timestamp;
    bionet_GeneralizedTime_to_timeval(&newHab->timestamp, &timestamp);
    bionet_event = bionet_event_new(&timestamp, (const char*)newHab->bdmId.buf, BIONET_EVENT_PUBLISHED);
    if (bionet_event == NULL) {
        // an error has been logged already
        return;
    }
    bionet_hab_add_event(hab, bionet_event);

    if (libbdm_callback_new_hab != NULL) {
        libbdm_callback_new_hab(hab, libbdm_callback_new_hab_usr_data);
    }
}

static void bdm_handle_new_node(const cal_event_t *event, const BDMNewNode_t *newNode) {
    bionet_hab_t *hab;
    bionet_node_t *node, *tmp_node;
    bionet_event_t * bionet_event;
    uint8_t node_uid[BDM_UUID_LEN];

    hab = bdm_cache_lookup_hab((const char*)newNode->habType.buf, (const char*)newNode->habId.buf);
    if (hab == NULL) {
        hab = bionet_hab_new((const char*)newNode->habType.buf, (const char*)newNode->habId.buf);
        if (hab == NULL) {
            // an error has been logged already
            return;
        }
        libbdm_cache_add_hab(hab);
    }

    // There is no UID in BDMNewNode, so calculate it
    tmp_node = bionet_asn_to_node(&newNode->node, hab);
    if (tmp_node == NULL) {
        // an error has been logged
        return;
    }
    db_make_node_guid(tmp_node, node_uid);


    // Get and use existing node, or add the tmp_node
    node = bionet_hab_get_node_by_id_and_uid(hab, bionet_node_get_id(tmp_node), node_uid);
    if( NULL == node ) {
        bionet_node_set_uid(tmp_node, node_uid);
        bionet_hab_add_node(hab, tmp_node);
        node = tmp_node;
        libbdm_cache_add_node(node);
    } else {
        bionet_node_free(tmp_node);
    }

    struct timeval timestamp;
    bionet_GeneralizedTime_to_timeval(&newNode->timestamp, &timestamp);
    bionet_event = bionet_event_new(&timestamp, (const char*)newNode->bdmId.buf, BIONET_EVENT_PUBLISHED);
    if (bionet_event == NULL) {
        // an error has been logged already
        return;
    }
    bionet_node_add_event(node, bionet_event);

    if (libbdm_callback_new_node != NULL) {
        libbdm_callback_new_node(node, libbdm_callback_new_node_usr_data);
    }
}


static void bdm_handle_lost_hab(const cal_event_t *event, const BDMLostHab_t *lostHab) {
    bionet_hab_t *hab;
    bionet_event_t * bionet_event;

    hab = bdm_cache_lookup_hab((const char*)lostHab->habType.buf, (const char*)lostHab->habId.buf);
    if (hab == NULL) {
        hab = bionet_hab_new((const char*)lostHab->habType.buf, (const char*)lostHab->habId.buf);
        if (hab == NULL) {
            // an error has been logged already
            return;
        }
        libbdm_cache_add_hab(hab);
    }

    struct timeval timestamp;
    bionet_GeneralizedTime_to_timeval(&lostHab->timestamp, &timestamp);
    bionet_event = bionet_event_new(&timestamp, (const char*)lostHab->bdmId.buf, BIONET_EVENT_LOST);
    if (bionet_event == NULL) {
        // an error has been logged already
        return;
    }
    bionet_hab_add_event(hab, bionet_event);

    if (libbdm_callback_lost_hab != NULL) {
        libbdm_callback_lost_hab(hab, libbdm_callback_lost_hab_usr_data);
    }
}

static void bdm_handle_lost_node(const cal_event_t *event, const BDMLostNode_t *lostNode) {
    bionet_node_t *node;
    bionet_event_t * bionet_event;

    if(lostNode->uid.size != BDM_UUID_LEN){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s(): asn syntax error: bad uuid length", __FUNCTION__);
        return;
    }

    node = bdm_cache_lookup_node_uid(lostNode->uid.buf);
    if (node == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s(): Cannot find node by uuid " UUID_FMTSTR, __FUNCTION__,
                UUID_ARGS(lostNode->uid.buf));
        return;
    }

    struct timeval timestamp;
    bionet_GeneralizedTime_to_timeval(&lostNode->timestamp, &timestamp);
    bionet_event = bionet_event_new(&timestamp, (const char*)lostNode->bdmId.buf, BIONET_EVENT_LOST);
    if (bionet_event == NULL) {
        // an error has been logged already
        return;
    }
    bionet_node_add_event(node, bionet_event);

    if (libbdm_callback_lost_node != NULL) {
        libbdm_callback_lost_node(node, libbdm_callback_lost_node_usr_data);
    }
}


static void bdm_handle_resource_metadata(const cal_event_t *event, const BDMResourceMetadata_t *rm) {
    bionet_hab_t *hab;
    bionet_node_t *node;
    bionet_resource_t *resource;

    const char * hab_type = (const char*)rm->habType.buf;
    const char * hab_id = (const char*)rm->habId.buf;
    const char * node_id = (const char*)rm->nodeId.buf;
    const char * resource_id = (const char*)rm->resourceId.buf;
    uint8_t node_uid[BDM_UUID_LEN];


    libbdm_peer_t * bdm_state = g_hash_table_lookup(libbdm_all_peers, event->peer_name);
    if (NULL == bdm_state) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "message from unknown peer %s", event->peer_name);
        return;
    }
    
    if(rm->nodeUid.size != sizeof(node_uid)) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "message corrupt. Bad node UID length");
	return;
    }
    memcpy(node_uid, rm->nodeUid.buf, sizeof(node_uid));

    hab = bdm_cache_lookup_hab(hab_type, hab_id);
    if (hab == NULL) {
        hab = bionet_hab_new(hab_type, hab_id);
        if (hab == NULL) {
            // an error has been logged already
            return;
        }
        libbdm_cache_add_hab(hab);
    }

    node = bionet_hab_get_node_by_id_and_uid(hab, node_id, node_uid);
    if (node == NULL) {
        node = bionet_node_new(hab, node_id);
        if (node == NULL) {
            // an error has been logged already
            return;
        }

        bionet_node_set_uid(node, node_uid);
        bionet_hab_add_node(hab, node);

        libbdm_cache_add_node(node);
    }

    resource = bdm_cache_lookup_resource_uid(node_uid, resource_id);
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
        bionet_node_add_resource(node, resource);

        libbdm_cache_add_resource(resource);
    }
}

static void bdm_handle_resource_datapoints(const cal_event_t *event, BDMResourceDatapoints_t *rd) {
    bionet_resource_t *resource;
    int i;
    uint8_t node_uid[BDM_UUID_LEN];

    const char * resource_id = (const char*)rd->resourceId.buf;

    if(rd->nodeUid.size != sizeof(node_uid)) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "message corrupt. Bad node UID length");
	return;
    }
    memcpy(node_uid, rd->nodeUid.buf, sizeof(node_uid));


    libbdm_peer_t * bdm_state = g_hash_table_lookup(libbdm_all_peers, event->peer_name);
    if (NULL == bdm_state) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "message from unknown peer %s", event->peer_name);
	return;
    }


    resource = bdm_cache_lookup_resource_uid(node_uid, resource_id);
    if (resource == NULL) {
        g_log(
            BDM_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "got a Datapoint for unknown Resource " UUID_FMTSTR ":%s",
            UUID_ARGS(node_uid), resource_id
        );
        return;
    }

    for (i = 0; i < rd->newDatapointsBDM.list.count; i ++) {
        BDMPublishDatapoint_t *asn_pd = rd->newDatapointsBDM.list.array[i];
        long seq;
        bionet_datapoint_t *new_d = bdm_publish_asn_to_datapoint(asn_pd, resource, &seq);

        if( NULL == new_d ){
            // An error has been logged already
            continue;
        }

        if(seq > bdm_state->curr_seq) {
            bdm_state->curr_seq = seq;
        }


        bionet_resource_remove_datapoint_by_index(resource, 0);
        bionet_resource_add_datapoint(resource, new_d);

        if(libbdm_callback_datapoint) {
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
            BDM_LOG_DOMAIN,
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

    libbdm_peer_t * bdm_state = g_hash_table_lookup(libbdm_all_peers, peer_name);
    if (NULL == bdm_state) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "message from unknown peer %s", event->peer_name);
	return;
    }

    memset(&m, 0x00, sizeof(BDM_C2S_Message_t));
    memset(&buf, 0, sizeof(buf));

    // If this is a new subscription being sent to this peer, send the new sequence number
    // if this is just a re-connect, then send the global peer curr_seq
    // 
    // If the topic is present with a sequence number >=0 then the subscription is
    // in progress. It will be set to -1 when the subscription is complete and the server
    // sends the state.
    long send_seq = bdm_state->curr_seq +1;
    long *plong;
    if ( (plong = g_hash_table_lookup(bdm_state->new_seq_by_topic, topic)) ) {
        if ( *plong >= 0 ) {
            send_seq = *plong;
        }
    } else {
        plong = malloc(sizeof(long));
        *plong = 0; // TODO: Query the user for the sequence number to use
        g_hash_table_insert(bdm_state->new_seq_by_topic, strdup(topic), plong );
        send_seq = -1;
    }

    m.present = BDM_C2S_Message_PR_sendState;
    m.choice.sendState.seq = send_seq;

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
    r = cal_client.sendto(libbdm_cal_handle, peer_name, buf.buf, buf.size);

    return;

cleanup:
    free(buf.buf);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_C2S_Message, &m);
}

static void bdm_handle_server_state_update(const cal_event_t *event, BDMSendState_t *state) {
    long *plong;
    const char * topic = (const char*)state->topic.buf;

    libbdm_peer_t * bdm_state = g_hash_table_lookup(libbdm_all_peers, event->peer_name);
    if (NULL == bdm_state) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "message from unknown peer %s", event->peer_name);
        return;
    }

    if ( (plong = g_hash_table_lookup(bdm_state->new_seq_by_topic, topic)) ) {
        *plong = -1; // Use the global value from now on
    }

    if(state->seq > bdm_state->curr_seq) {
        bdm_state->curr_seq = state->seq;
    }
}

static void bdm_handle_server_message(const cal_event_t *event) {
    BDM_S2C_Message_t *m = NULL;
    asn_dec_rval_t rval;

    rval = ber_decode(NULL, &asn_DEF_BDM_S2C_Message, (void **)&m, event->msg.buffer, event->msg.size);
    if (rval.code == RC_WMORE) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "server message from '%s' contained an incomplete ASN.1 message", event->peer_name);
        return;
    } else if (rval.code == RC_FAIL) {
        // received invalid junk
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "server message from '%s' contained an invalid ASN.1 message", event->peer_name);
        return;
    } else if (rval.code != RC_OK) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "unknown error (code=%d) decoding server message from '%s'", rval.code, event->peer_name);
        return;
    }

    if (rval.consumed != event->msg.size) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "server message from '%s' contained junk at end of message (consumed %d of %d)", event->peer_name, (int)rval.consumed, event->msg.size);
    }


    switch (m->present) {

        case BDM_S2C_Message_PR_newHab: {
            bdm_handle_new_hab(event, &m->choice.newHab);
            break;
        }

        case BDM_S2C_Message_PR_lostHab: {
            bdm_handle_lost_hab(event, &m->choice.lostHab);
            break;
        }

        case BDM_S2C_Message_PR_newNode: {
            bdm_handle_new_node(event, &m->choice.newNode);
            break;
        }

        case BDM_S2C_Message_PR_lostNode: {
            bdm_handle_lost_node(event, &m->choice.lostNode);
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
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "dont know what to do with BDM_S2C message type %d from %s", m->present, event->peer_name);
            xer_fprint(stdout, &asn_DEF_BDM_S2C_Message, m);
            break;
        }

    }

    asn_DEF_BDM_S2C_Message.free_struct(&asn_DEF_BDM_S2C_Message, m, 0);
}


void libbdm_cal_callback(void * cal_handle, const cal_event_t *event) {
    switch (event->type) {
        case CAL_EVENT_JOIN: {
            libbdm_peer_t *bdm_peer;

            // add the bdm to the bdm library's list of known bdms
	    bdm_peer = g_hash_table_lookup( libbdm_all_peers, event->peer_name );
            if ( NULL == bdm_peer ) {

                bdm_peer = calloc(1, sizeof(libbdm_peer_t));
                if (NULL == bdm_peer) {
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                          "Out of memory!");
                    return;
                }
                bdm_peer->curr_seq = -1;
                bdm_peer->new_seq_by_topic = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
                g_hash_table_insert(libbdm_all_peers, strdup(event->peer_name), bdm_peer);
            }

            bionet_bdm_t * bdm = bdm_cache_lookup_bdm(event->peer_name);
            if (bdm == NULL) {
                bdm = bionet_bdm_new(event->peer_name);
                if (bdm == NULL) {
                    // an error has been logged already
                    return;
                }
                libbdm_cache_add_bdm(bdm);
            }
            // see if we need to publish this to the user
            {
                GSList *i;
                for (i = libbdm_bdm_subscriptions; i != NULL; i = i->next) {
                    libbdm_bdm_subscription_t *sub = i->data;

                    if (bionet_name_component_matches(event->peer_name, sub->peer_id))
                    {
                        if (libbdm_callback_new_bdm != NULL) {
                            libbdm_callback_new_bdm(bdm, libbdm_callback_new_bdm_usr_data);
                        }
                        break;
                    }
                }
            }

            // TODO: Implement BDM cal-security
	    //bionet_bdm_set_secure(bdm_peer, event->is_secure);
            break;
        }

        case CAL_EVENT_LEAVE: {
            // Note, we keep a reference to all peers, to maintain state
            //libbdm_peer_t * peer = g_hash_table_lookup(libbdm_all_peers, event->peer_name);

            // However, do remove the peer from the cache, and notify the user
            bionet_bdm_t * bdm = bdm_cache_lookup_bdm(event->peer_name);
            if ( bdm ) { 
                libbdm_cache_remove_bdm(bdm); // This will call callbacks
            }

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
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "CAL unknown event type %d", event->type);
            break;
        }
    }
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
