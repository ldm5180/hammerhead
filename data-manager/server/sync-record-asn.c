
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#include <glib.h>

#include "bionet.h"
#include "bionet-util.h"
#include "bionet-data-manager.h"
#include "bdm-list-iterator.h"
#include "bdm-db.h"

#include "bdm-asn.h"

#include "util/protected.h"

static int md_handle_bdm(bionet_bdm_t * bdm, void * usr_data)
{
    md_iter_state_t * state = (md_iter_state_t*)usr_data;
    int r;
    DataManager_t * asn_bdm;

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
          " BDM %s:", bionet_bdm_get_id(bdm));

    //add the BDM to the message
    asn_bdm = (DataManager_t *)calloc(1, sizeof(DataManager_t));
    if (asn_bdm == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
              "%s(): out of memory!", __FUNCTION__);
        return -1;
    }

    r = asn_sequence_add(&state->asn_message->list, asn_bdm);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
              "sync_send_metadata(): error adding BDM to Sync Metadata: %s", strerror(errno));
        free(asn_bdm);
        return -1;
    }

    r = OCTET_STRING_fromString(&asn_bdm->id, bionet_bdm_get_id(bdm));
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
              "%s(): error making OCTET_STRING for BDM-ID %s", __FUNCTION__, bionet_bdm_get_id(bdm));
        return -1;
    }

    if(state->mtu > 0){
        ssize_t serialized_size = der_encoded_size(
                &asn_DEF_BDM_Sync_Message, state->asn_sync_message);
        if(serialized_size > state->mtu) {
            int i = state->asn_message->list.count - 1;
            asn_sequence_del(&state->asn_message->list, i, 1);

            ssize_t old_serialized_size = der_encoded_size(
                    &asn_DEF_BDM_Sync_Message, state->asn_sync_message);
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                  "%s(): Adding element %d makes size %" PRIdPTR " from %" PRIdPTR ", which would exceed mtu",
                  __FUNCTION__, i, serialized_size, old_serialized_size);
            return 1; // Tell caller send message and continue
        }
    }
        
    state->asn_bdm = asn_bdm;

    return 0;
}

static int md_handle_hab(
        bionet_bdm_t * bdm,
        bionet_hab_t * hab, 
        void * usr_data)
{
    md_iter_state_t * state = (md_iter_state_t*)usr_data;
    int r;
    int ei;

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
          "     HAB %s:", bionet_hab_get_name(hab));


    //add the HAB to the message
    BDM_HardwareAbstractor_t *asn_hab = (BDM_HardwareAbstractor_t *)calloc(1, sizeof(BDM_HardwareAbstractor_t));
    if (asn_hab == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
              "%s(): out of memory!", __FUNCTION__);
        return -1;
    }

    r = asn_sequence_add(&state->asn_bdm->hablist.list, asn_hab);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
              "%s(): error adding HAB to Metadata: %s", __FUNCTION__, strerror(errno));
        free(asn_hab);
        return -1;
    }

    r = OCTET_STRING_fromString(&asn_hab->type, bionet_hab_get_type(hab));
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s(): error making OCTET_STRING for HAB-Type %s",
                __FUNCTION__, bionet_hab_get_type(hab));
        return -1;
    }

    r = OCTET_STRING_fromString(&asn_hab->id, bionet_hab_get_id(hab));
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                "%s(): error making OCTET_STRING for HAB-ID %s",
                __FUNCTION__, bionet_hab_get_id(hab));
        return -1;
    }

    // Add new/lost hab events
    for (ei = 0; ei < bionet_hab_get_num_events(hab); ei++) {
        bionet_event_t * event = bionet_hab_get_event_by_index(hab, ei);

        BDM_Event_t * asn_event = calloc(1, sizeof(BDM_Event_t));
        if(NULL == asn_event) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Out of memory");
            return -1;
        }

        r = asn_sequence_add(&asn_hab->events.list, asn_event);
        if (r != 0) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                  "bdm_to_asn(): error adding BDM to ASN Metadata: %s", strerror(errno));
            free(asn_event);
            return -1;
        }

        r = bionet_event_to_asn_r(event, asn_event);
        if(r) return -1;
    }

    if(state->mtu > 0){
        ssize_t serialized_size = der_encoded_size(
                &asn_DEF_BDM_Sync_Message, state->asn_sync_message);
        if(serialized_size > state->mtu) {
            int i = state->asn_bdm->hablist.list.count -1;
            asn_sequence_del(&state->asn_bdm->hablist.list, i, 1);

            ssize_t old_serialized_size = der_encoded_size(
                    &asn_DEF_BDM_Sync_Message, state->asn_sync_message);

            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                  "%s(): Adding element %d makes size %" PRIdPTR " from %" PRIdPTR ", which would exceed mtu",
                  __FUNCTION__, i, serialized_size, old_serialized_size);
            return 1; // Tell caller send message and continue here
        }
    }
        

    state->asn_hab = asn_hab;

    return 0;
}

static int md_handle_node(
        bionet_bdm_t * bdm,
        bionet_node_t * node, 
        void * usr_data)
{
    md_iter_state_t * state = (md_iter_state_t*)usr_data;

    BDM_Node_t *asn_node;

    int r;

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
          "         NODE %s:", bionet_node_get_id(node));


    asn_node = bionet_node_to_bdm_asn(node);
    if (NULL == asn_node) return -1;

    r = asn_sequence_add(&state->asn_hab->nodes.list, asn_node);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error adding Node to ResourceDatapointReply: %s", __FUNCTION__, strerror(errno));
        ASN_STRUCT_FREE(asn_DEF_BDM_Node, asn_node);
        return -1;
    }

    if(state->mtu > 0){
        ssize_t serialized_size = der_encoded_size(
                &asn_DEF_BDM_Sync_Message, state->asn_sync_message);
        if(serialized_size > state->mtu) {
            int i = state->asn_hab->nodes.list.count -1;
            asn_sequence_del(&state->asn_hab->nodes.list, i, 1);

            ssize_t old_serialized_size = der_encoded_size(
                    &asn_DEF_BDM_Sync_Message, state->asn_sync_message);
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                  "%s(): Adding element %d makes size %" PRIdPTR " from %" PRIdPTR ", which would exceed mtu",
                  __FUNCTION__, i, serialized_size, old_serialized_size);
            return 1; // Tell caller send message and continue
        }
    }
        

    state->asn_node = asn_node;

    return 0;
}

void bdm_sync_metadata_to_asn_setup(
        GPtrArray * bdm_list,
        ssize_t mtu,
        sqlite_int64 recipient_key,
        sqlite_int64 firstSeq,
        sqlite_int64 lastSeq,
        md_iter_state_t * state_buf,
        bdm_list_iterator_t * iter_buf)
{
    memset(state_buf, 0, sizeof(md_iter_state_t));
    state_buf->mtu = mtu; // Subtract the sync_message wrapper size
    state_buf->channid = recipient_key; 
    state_buf->firstSeq = firstSeq;
    state_buf->lastSeq = lastSeq;

    bdm_iterator_init(bdm_list, 
            md_handle_bdm,
            md_handle_hab,
            md_handle_node,
            NULL,
            NULL,
            (void *)state_buf,
            iter_buf
            );
}

BDM_Sync_Message_t * bdm_sync_metadata_to_asn(
        bdm_list_iterator_t * iter,
        md_iter_state_t * state)
{

    BDM_Sync_Metadata_Message_t * message;
    BDM_Sync_Message_t *sync_message;
    int r;

    if(state->done) {
        return NULL;
    }

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
          "Building Metadata Sync Message {");

    sync_message = (BDM_Sync_Message_t*)calloc(1, sizeof(BDM_Sync_Message_t));
    if (NULL == sync_message) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "%s(): Out of Memory!", __FUNCTION__);
        return NULL;
    }

    sync_message->syncchannel = state->channid;
    sync_message->firstSeq = state->firstSeq;
    sync_message->lastSeq = state->lastSeq;
    sync_message->data.present = BDM_Sync_Data_PR_metadataMessage;
    message = &sync_message->data.choice.metadataMessage;

    state->asn_sync_message = sync_message;
    state->asn_message = message;

    r = bdm_list_traverse(iter);
    if(r < 0) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "%s(): Error while traversing bdm_list", __FUNCTION__);
        ASN_STRUCT_FREE(asn_DEF_BDM_Sync_Message, sync_message);
        return NULL;
    }

    if ( r == 0 ) {
        // Done done.
        state->done = 1;
    }

    size_t message_size = der_encoded_size(
                &asn_DEF_BDM_Sync_Metadata_Message, message);

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
          "} Done Building Metadata Sync Message (%" PRIdPTR " bytes)",
          message_size);


    return sync_message;
} 

/*** DATAPOINTS ***/


static int dp_handle_bdm(bionet_bdm_t * bdm, void * usr_data)
{
    dp_iter_state_t * state = (dp_iter_state_t*)usr_data;
    BDM_Sync_Datapoints_Message_t * message = state->asn_message;

    int r;

    BDMSyncRecord_t * sync_record = (BDMSyncRecord_t *)calloc(1, sizeof(BDMSyncRecord_t));
    if (NULL == sync_record) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
              "sync_send_datapoints(): Failed to malloc sync_record: %m");
        return -1;
    }

    r = asn_sequence_add(&message->list, sync_record);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
              "sync_send_datapoints(): error adding sync record to BDM Sync Datapoints Message: %m");
        free(sync_record);
        return -1;
    }


    //add BDM-ID to BDM Sync Record, not just a NULL
    r = OCTET_STRING_fromString(&sync_record->bdmID, bionet_bdm_get_id(bdm));
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "sync_send_datapoints(): Failed to set BDM ID");
        return -1;
    }

    if(state->mtu > 0){
        ssize_t serialized_size = der_encoded_size(
                &asn_DEF_BDM_Sync_Message, state->asn_sync_message);
        if(serialized_size > state->mtu) {
            int i = state->asn_message->list.count -1;
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                  "%s(): Adding element %d makes size %" PRIdPTR ", which would exceed mtu",
                  __FUNCTION__, i, serialized_size);
            asn_sequence_del(&state->asn_message->list, i, 1);
            return 1; // Tell caller send message and continue
        }
    }
        

    state->asn_sync_record = sync_record;


    return 0;
}


static int dp_handle_resource(
        bionet_bdm_t * bdm,
        bionet_resource_t * resource, 
        void * usr_data)
{
    dp_iter_state_t * state = (dp_iter_state_t*)usr_data;
    int r;

    ResourceRecord_t * resource_rec = 
        (ResourceRecord_t*)calloc(1, sizeof(ResourceRecord_t));
    if (NULL == resource_rec) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Out of Memory!", __FUNCTION__);
        return -1;
    }

    r = asn_sequence_add(&state->asn_sync_record->syncResources.list, resource_rec);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "sync_send_datapoints(): Failed to add resource record.");
        free(resource_rec);
        return -1;
    }

    bionet_node_t * node = bionet_resource_get_node(resource);
    bionet_hab_t * hab = bionet_node_get_hab(node);
    uint8_t resource_key[BDM_RESOURCE_KEY_LENGTH];
    db_make_resource_key(
        bionet_hab_get_type(hab),
        bionet_hab_get_id(hab),
        bionet_node_get_id(node),
        bionet_resource_get_id(resource),
        bionet_resource_get_data_type(resource),
        bionet_resource_get_flavor(resource),
        resource_key);
    r = OCTET_STRING_fromBuf(&resource_rec->resourceKey, (const char *)resource_key, BDM_RESOURCE_KEY_LENGTH);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "sync_send_datapoints(): Failed to set resource key");
        return -1;
    }

    if(state->mtu > 0){
        ssize_t serialized_size = der_encoded_size(
                &asn_DEF_BDM_Sync_Message, state->asn_sync_message);
        if(serialized_size > state->mtu) {
            int i = state->asn_sync_record->syncResources.list.count -1;
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                  "%s(): Adding element %d makes size %" PRIdPTR ", which would exceed mtu",
                  __FUNCTION__, i, serialized_size);
            asn_sequence_del(&state->asn_sync_record->syncResources.list, i, 1);
            return 1; // Tell caller send message and continue
        }
    }
        

    state->asn_resource_rec = resource_rec;

    return 0;
}

static int dp_handle_datapoint(
        bionet_bdm_t * bdm,
        bionet_datapoint_t * d, 
        void * usr_data)
{
    dp_iter_state_t * state = (dp_iter_state_t*)usr_data;

    int r;

    bionet_resource_t * resource = bionet_datapoint_get_resource(d);

    int ei;
    for (ei=0; ei < bionet_datapoint_get_num_events(d); ei++) {
        bionet_event_t * event = bionet_datapoint_get_event_by_index(d, ei);

        {
            char * time_str = bionet_event_get_timestamp_as_str(event);

            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
                  "        + Datapoint for %s @ %s [" UUID_FMTSTR "]",
                    bionet_resource_get_name(resource),
                    time_str,
                    UUID_ARGS(state->asn_resource_rec->resourceKey.buf)
                    );

            free(time_str);
        }


        BDMSyncDatapoint_t * asn_sync_datapoint = calloc(1, sizeof(BDMSyncDatapoint_t));
        if (asn_sync_datapoint == NULL) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                  "send_sync_datapoints(): out of memory!");
            return -1;
        }

        bionet_timeval_to_GeneralizedTime(
                bionet_event_get_timestamp(event), &asn_sync_datapoint->timestamp);

        asn_sync_datapoint->entrySeq = bionet_event_get_seq(event);
        
        Datapoint_t *asn_datapoint = bionet_datapoint_to_asn_r(d, &asn_sync_datapoint->datapoint);
        if (asn_datapoint == NULL) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                  "send_sync_datapoints(): out of memory!");
            return -1;
        }
        
        r = asn_sequence_add(&state->asn_resource_rec->resourceDatapoints.list, asn_sync_datapoint);
        if (r != 0) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                  "send_sync_datapoints(): error adding Datapoint to Resource: %m");
            free(asn_sync_datapoint);
            return -1;
        }


        if(state->mtu > 0){
            ssize_t serialized_size = der_encoded_size(
                    &asn_DEF_BDM_Sync_Message, state->asn_sync_message);
            if(serialized_size > state->mtu) {
                int i = state->asn_resource_rec->resourceDatapoints.list.count -1;
                asn_sequence_del(&state->asn_resource_rec->resourceDatapoints.list, i, 1);

                ssize_t old_serialized_size = der_encoded_size(
                    &asn_DEF_BDM_Sync_Message, state->asn_sync_message);
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                  "%s(): Adding element %d,%d makes size %" PRIdPTR " from %" PRIdPTR ", which would exceed mtu",
                      __FUNCTION__, ei, i,serialized_size, old_serialized_size);
                return 1; // Tell caller send message and continue
            }
        }

    } // for each event

    return 0;
}

void bdm_sync_datapoints_to_asn_setup(
        GPtrArray * bdm_list,
        ssize_t mtu,
        sqlite_int64 recipient_key,
        sqlite_int64 firstSeq,
        sqlite_int64 lastSeq,
        dp_iter_state_t * state_buf,
        bdm_list_iterator_t * iter_buf)
{
    memset(state_buf, 0, sizeof(dp_iter_state_t));
    state_buf->mtu = mtu; // Subtract the sync_message wrapper size
    state_buf->channid = recipient_key; 
    state_buf->firstSeq = firstSeq;
    state_buf->lastSeq = lastSeq;

    bdm_iterator_init(bdm_list, 
            dp_handle_bdm,
            NULL,
            NULL,
            dp_handle_resource,
            dp_handle_datapoint,
            (void *)state_buf,
            iter_buf
            );
}

BDM_Sync_Message_t * bdm_sync_datapoints_to_asn(bdm_list_iterator_t * iter, dp_iter_state_t * state)
{

    BDM_Sync_Message_t *sync_message;
    BDM_Sync_Datapoints_Message_t * message;
    int r;

    if (NULL == iter->bdm_list) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "%s(): NULL BDM list", __FUNCTION__);
        return NULL;
    }

    if(state->done) {
        return NULL;
    }


    sync_message = (BDM_Sync_Message_t*)calloc(1, sizeof(BDM_Sync_Message_t));
    if (NULL == sync_message) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "%s(): Out of Memory!", __FUNCTION__);
        return NULL;
    }

    sync_message->firstSeq = state->firstSeq;
    sync_message->lastSeq = state->lastSeq;
    sync_message->syncchannel = state->channid;
    sync_message->data.present = BDM_Sync_Data_PR_datapointsMessage;
    message = &sync_message->data.choice.datapointsMessage;

    state->asn_sync_message = sync_message;
    state->asn_message = message;

    r = bdm_list_traverse(iter);
    if(r < 0) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "%s(): Error while traversing bdm_list", __FUNCTION__);
        ASN_STRUCT_FREE(asn_DEF_BDM_Sync_Message, sync_message);
        return NULL;
    }

    if ( 0 == r ) {
        // Done Done
        state->done = 1;
    }

    return sync_message;
} 


int bdm_gen_sync_msg_ack_asnbuf(BDM_Sync_Message_t * msg, bionet_asn_buffer_t *buf) {

    BDM_Sync_Message_t ackmsg;
    asn_enc_rval_t asn_r;

    switch(msg->data.present) {
        case BDM_Sync_Data_PR_datapointsMessage:
            ackmsg.data.present = BDM_Sync_Data_PR_ackDatapoints;
            ackmsg.data.choice.ackDatapoints = 1;
            break;

        case BDM_Sync_Data_PR_metadataMessage:
            ackmsg.data.present = BDM_Sync_Data_PR_ackMetadata;
            ackmsg.data.choice.ackMetadata = 1;
            break;

        default:
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                    "%s(): Unknown message type", __FUNCTION__);
            return -1;
    }
    ackmsg.syncchannel = msg->syncchannel;
    ackmsg.firstSeq = msg->firstSeq;
    ackmsg.lastSeq = msg->lastSeq;


    buf->buf = NULL;
    buf->size = 0;

    asn_r = der_encode(&asn_DEF_BDM_Sync_Message, &ackmsg, bionet_accumulate_asn_buffer, buf);
    if (asn_r.encoded == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s(): error with der_encode(): %s", __FUNCTION__, strerror(errno));
        if (buf->buf != NULL) {
            free(buf->buf);
            buf->buf = NULL;
        }
        return -1;
    }

    return 0;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
