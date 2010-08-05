
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
#include "bdm-db.h"

#include "bdm-asn.h"

#include "util/protected.h"


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

BDM_Sync_Message_t * bdm_sync_datapoints_to_asn(GPtrArray * bdm_list)
{

    BDM_Sync_Message_t *sync_message;
    BDM_Sync_Datapoints_Message_t * message;
    int r, bi;

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

    sync_message->present = BDM_Sync_Message_PR_datapointsMessage;
    message = &sync_message->choice.datapointsMessage;


    //create a sync record for each BDM
    for (bi = 0; bi < bdm_list->len; bi++) {
	int hi;
	bionet_bdm_t * bdm = g_ptr_array_index(bdm_list, bi);
	if (NULL == bdm) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "Failed to get BDM %d from BDM list", bi);
	    goto cleanup;
	}

	BDMSyncRecord_t * sync_record = (BDMSyncRecord_t *)calloc(1, sizeof(BDMSyncRecord_t));
	if (sync_record) {
	    r = asn_sequence_add(&message->list, sync_record);
	    if (r != 0) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
		      "sync_send_datapoints(): error adding sync record to BDM Sync Datapoints Message: %m");
	    }
	} else {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
		  "sync_send_datapoints(): Failed to malloc sync_record: %m");
            goto cleanup;
	}
    
    
	//add BDM-ID to BDM Sync Record, not just a NULL
	r = OCTET_STRING_fromString(&sync_record->bdmID, bionet_bdm_get_id(bdm));
	if (r != 0) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "sync_send_datapoints(): Failed to set BDM ID");
            goto cleanup;
	}
    
    
	//walk list of habs
        GPtrArray * hab_list = bionet_bdm_get_hab_list(bdm);
	for (hi = 0; hi < hab_list->len; hi++) {
	    int ni;
	    bionet_hab_t * hab = g_ptr_array_index(hab_list, hi);
	    if (NULL == hab) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
		      "Failed to get HAB %d from array of HABs", hi);
                goto cleanup;
	    }
	    
	    //walk list of nodes
	    for (ni = 0; ni < bionet_hab_get_num_nodes(hab); ni++) {
		int ri;
		bionet_node_t * node = bionet_hab_get_node_by_index(hab, ni);
		if (NULL == node) {
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
			  "Failed to get node %d from HAB %s", ni, bionet_hab_get_name(hab));
                    goto cleanup;
		}
		
		
		//walk list of resources
		for (ri = 0; ri < bionet_node_get_num_resources(node); ri++) {
		    int di;
		    ResourceRecord_t * resource_rec = 
                        (ResourceRecord_t*)calloc(1, sizeof(ResourceRecord_t));
		    bionet_resource_t * resource = bionet_node_get_resource_by_index(node, ri);
		    if (NULL == resource) {
			g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
			      "Failed to get resource %d from Node %s", ri, bionet_node_get_name(node));
		    }
		    
		    r = asn_sequence_add(&sync_record->syncResources, resource_rec);
		    if (r != 0) {
			g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
			      "sync_send_datapoints(): Failed to add resource record.");
		    }

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
                        goto cleanup;
		    }
		    
		    //walk list of datapoints and add each one to the message
		    for (di = 0; di < bionet_resource_get_num_datapoints(resource); di++) {
                        int ei;
			bionet_datapoint_t * d = bionet_resource_get_datapoint_by_index(resource, di);
			if (NULL == d) {
			    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
				  "Failed to get datapoint %d from Resource %s", di, bionet_resource_get_name(resource));
                            goto cleanup;
			}

                        for ( ei = 0; ei < bionet_datapoint_get_num_events(d); ei++) {
                            bionet_event_t * event = bionet_datapoint_get_event_by_index(d, ei);
                            char * time_str = bionet_event_get_timestamp_as_str(event);

                            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
                                  "        + Datapoint for %s.%s.%s:%s @ %s [" UUID_FMTSTR "]",
                                    bionet_hab_get_type(hab),
                                    bionet_hab_get_id(hab),
                                    bionet_node_get_id(node),
                                    bionet_resource_get_id(resource),
                                    time_str,
                                    UUID_ARGS(resource_key)
                                    );

                            free(time_str);


                            BDMSyncDatapoint_t * asn_sync_datapoint = calloc(1, sizeof(BDMSyncDatapoint_t));
                            if (asn_sync_datapoint == NULL) {
                                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                                      "send_sync_datapoints(): out of memory!");
                                goto cleanup;
                            }

                            bionet_timeval_to_GeneralizedTime(
                                    bionet_event_get_timestamp(event), &asn_sync_datapoint->timestamp);

                            asn_sync_datapoint->entrySeq = bionet_event_get_seq(event);
                            
                            Datapoint_t *asn_datapoint = bionet_datapoint_to_asn_r(d, &asn_sync_datapoint->datapoint);
                            if (asn_datapoint == NULL) {
                                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                                      "send_sync_datapoints(): out of memory!");
                                goto cleanup;
                            }
                            
                            r = asn_sequence_add(&resource_rec->resourceDatapoints.list, asn_datapoint);
                            if (r != 0) {
                                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                                      "send_sync_datapoints(): error adding Datapoint to Resource: %m");
                                goto cleanup;
                            }
                        } // for each event
		    } //for each datapoint
		} //for each resource
	    } //for each node
	} //for each hab
    } //for each bdm

    if (bi) {
        return sync_message;
    }
 
cleanup:
    ASN_STRUCT_FREE(asn_DEF_BDM_Sync_Message, sync_message);
    return NULL;
} 

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
