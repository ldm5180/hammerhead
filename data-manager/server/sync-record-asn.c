
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "bionet.h"
#include "bionet-util.h"
#include "bionet-data-manager.h"
#include "bdm-list-iterator.h"
#include "bdm-db.h"

#include "bdm-asn.h"

#include "util/protected.h"

int md_bdm_cb(bionet_bdm_t * bdm, void * usr_data);

int md_hab_cb(
        bionet_bdm_t * bdm,
        bionet_hab_t * hab, 
        void * usr_data);

int md_node_cb(
        bionet_bdm_t * bdm,
        bionet_node_t * node, 
        void * usr_data);

BDM_Sync_Message_t * bdm_sync_metadata_to_asn(GPtrArray * bdm_list)
{

    BDM_Sync_Metadata_Message_t * message;
    BDM_Sync_Message_t *sync_message;
    int bi, r;

    if (NULL == bdm_list) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "%s(): NULL BDM list", __FUNCTION__);
        return NULL;
    }

    sync_message = (BDM_Sync_Message_t*)calloc(1, sizeof(BDM_Sync_Message_t));
    if (NULL == sync_message) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "%s(): Out of Memory!", __FUNCTION__);
        return NULL;
    }

    sync_message->present = BDM_Sync_Message_PR_metadataMessage;
    message = &sync_message->choice.metadataMessage;

    for (bi = 0; bi < bdm_list->len; bi++) {
        DataManager_t * asn_bdm;
	bionet_bdm_t * bdm = g_ptr_array_index(bdm_list, bi);
	if (NULL == bdm) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "Failed to get BDM %d from BDM list", bi);
	    goto cleanup;
	}
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
              "       BDM %s", bionet_bdm_get_id(bdm));

        //add the BDM to the message
        asn_bdm = bionet_bdm_to_asn(bdm);

        r = asn_sequence_add(&message->list, asn_bdm);
        if (r != 0) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                  "sync_send_metadata(): error adding BDM to Sync Metadata: %s", strerror(errno));
            goto cleanup;
        }
    } //for (bi = 0; bi < bdm_list->len; bi++)

    if (bi) {
        return sync_message;
    }

cleanup:
    ASN_STRUCT_FREE(asn_DEF_BDM_Sync_Message, sync_message);
    return NULL;
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

    r = asn_sequence_add(&state->asn_sync_record->syncResources, resource_rec);
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
    for ( ei = 0; ei < bionet_datapoint_get_num_events(d); ei++) {
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
    } // for each event

    return 0;
}

void bdm_sync_datapoints_to_asn_setup(
        GPtrArray * bdm_list,
        dp_iter_state_t * state_buf,
        bdm_list_iterator_t * iter_buf)
{
    memset(state_buf, 0, sizeof(dp_iter_state_t));

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

    sync_message = (BDM_Sync_Message_t*)calloc(1, sizeof(BDM_Sync_Message_t));
    if (NULL == sync_message) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "%s(): Out of Memory!", __FUNCTION__);
        return NULL;
    }

    sync_message->present = BDM_Sync_Message_PR_datapointsMessage;
    message = &sync_message->choice.datapointsMessage;

    state->asn_message = message;

    r = bdm_list_traverse(iter);
    if(r) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "%s(): Error while traversing bdm_list", __FUNCTION__);
        ASN_STRUCT_FREE(asn_DEF_BDM_Sync_Message, sync_message);
        return NULL;
    }

    return sync_message;
} 

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
