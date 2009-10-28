
// Copyright (c) 2008-2009, Regents of the University of Colorado.
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
#include "bdm-util.h"
#include "bionet-data-manager.h"

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
	int hi;
	bdm_t * bdm = g_ptr_array_index(bdm_list, bi);
	if (NULL == bdm) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "Failed to get BDM %d from BDM list", bi);
	    goto cleanup;
	}
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
              "       BDM %s", bdm->bdm_id);

	GPtrArray * hab_list = bdm->hab_list;
	if (NULL == hab_list) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "sync_send_metadata(): Failed to get HAB list from array of BDMs");
	    goto cleanup;
	}

	for (hi = 0; hi < hab_list->len; hi++) {
	    int ni;
	    HardwareAbstractor_t * asn_hab;
	    bionet_hab_t * hab = g_ptr_array_index(hab_list, hi);
	    if (NULL == hab) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
		      "sync_send_metadata(): Failed to get HAB %d from array of HABs", hi);
	    }
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
                  "         HAB %s", bionet_hab_get_id(hab));
	    
	    //add the HAB to the message
	    asn_hab = (HardwareAbstractor_t *)calloc(1, sizeof(HardwareAbstractor_t));
	    if (asn_hab == NULL) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "sync_send_metadata(): out of memory!");
		goto cleanup;
	    }
	    
	    r = asn_sequence_add(&message->list, asn_hab);
	    if (r != 0) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "sync_send_metadata(): error adding HAB to Sync Metadata: %s", strerror(errno));
		goto cleanup;
	    }
	    
	    r = OCTET_STRING_fromString(&asn_hab->type, bionet_hab_get_type(hab));
	    if (r != 0) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "sync_send_metadata(): error making OCTET_STRING for HAB-Type %s", bionet_hab_get_type(hab));
		goto cleanup;
	    }
	    
	    r = OCTET_STRING_fromString(&asn_hab->id, bionet_hab_get_id(hab));
	    if (r != 0) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "sync_send_metadata(): error making OCTET_STRING for HAB-ID %s", bionet_hab_get_id(hab));
		goto cleanup;
	    }
	    
	    for (ni = 0; ni < bionet_hab_get_num_nodes(hab); ni++) {
		int ri;
		Node_t * asn_node;
		bionet_node_t * node = bionet_hab_get_node_by_index(hab, ni);
		if (NULL == node) {
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
			  "sync_send_metadata(): Failed to get node %d from HAB %s", ni, bionet_hab_get_name(hab));
		}
		
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
                      "           NODE %s", bionet_node_get_id(node));

		//add the Node to the message
		asn_node = (Node_t *)calloc(1, sizeof(Node_t));
		if (asn_node == NULL) {
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "sync_send_metadata(): out of memory!");
		    goto cleanup;
		}
		
		r = asn_sequence_add(&asn_hab->nodes.list, asn_node);
		if (r != 0) {
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "sync_send_metadata(): error adding Node to Sync Metadata: %m");
		    goto cleanup;
		}
		
		r = OCTET_STRING_fromString(&asn_node->id, bionet_node_get_id(node));
		if (r != 0) {
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
			  "sync_send_metadata(): error making OCTET_STRING for Node-ID %s", bionet_node_get_id(node));
		    goto cleanup;
		}
		
		for (ri = 0; ri < bionet_node_get_num_resources(node); ri++) {
		    Resource_t * asn_resource;
		    bionet_resource_t * resource = bionet_node_get_resource_by_index(node, ri);
		    if (NULL == resource) {
			g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
			      "sync_send_metadata(): Failed to get resource %d from Node %s", ri, bionet_node_get_name(node));
		    }

                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
                          "             RES %s", bionet_resource_get_id(resource));
		    
		    //add the Resource to the message
		    asn_resource = (Resource_t *)calloc(1, sizeof(Resource_t));
		    if (asn_resource == NULL) {
			g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "sync_send_metadata(): out of memory!");
			goto cleanup;
		    }
		    
		    r = asn_sequence_add(&asn_node->resources.list, asn_resource);
		    if (r != 0) {
			g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
			      "sync_send_metadata(): error adding Resource to Sync Metadata: %m");
			goto cleanup;
		    }
		    
		    r = OCTET_STRING_fromString(&asn_resource->id, bionet_resource_get_id(resource));
		    if (r != 0) {
			g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
			      "sync_send_metadata(): error making OCTET_STRING for Resource-ID %s", bionet_resource_get_id(resource));
			goto cleanup;
		    }
		    
		    asn_resource->flavor = bionet_flavor_to_asn(bionet_resource_get_flavor(resource));
		    if (asn_resource->flavor == -1) {
			g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
			      "sync_send_metadata(): resource has invalid flavor");
			goto cleanup;
		    }
		    
		    asn_resource->datatype = bionet_datatype_to_asn(bionet_resource_get_data_type(resource));
		    if (asn_resource->datatype == -1) {
			g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
			      "sync_send_metadata(): resource has invalid datatype");
			goto cleanup;
		    }
		} //for (ri = 0; ri < bionet_node_get_num_resources(node); ri++)
	    } //for (ni = 0; ni < bionet_hab_get_num_nodes(hab); ni++)
	} //for (hi = 0; hi < hab_list->len; hi++)
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
	bdm_t * bdm = g_ptr_array_index(bdm_list, bi);
	if (NULL == bdm) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "Failed to get BDM %d from BDM list", bi);
	    goto cleanup;
	}

	GPtrArray * hab_list = bdm->hab_list;
	if (NULL == hab_list) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "sync_send_metadata(): Failed to get HAB list from array of BDMs");
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
	}
    
    
	//add BDM-ID to BDM Sync Record, not just a NULL
	r = OCTET_STRING_fromString(&sync_record->bdmID, bdm->bdm_id);
	if (r != 0) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "sync_send_datapoints(): Failed to set BDM ID");
	}
    
    
	//walk list of habs
	for (hi = 0; hi < hab_list->len; hi++) {
	    int ni;
	    bionet_hab_t * hab = g_ptr_array_index(hab_list, hi);
	    if (NULL == hab) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
		      "Failed to get HAB %d from array of HABs", hi);
	    }
	    
	    //walk list of nodes
	    for (ni = 0; ni < bionet_hab_get_num_nodes(hab); ni++) {
		int ri;
		bionet_node_t * node = bionet_hab_get_node_by_index(hab, ni);
		if (NULL == node) {
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
			  "Failed to get node %d from HAB %s", ni, bionet_hab_get_name(hab));
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
		    }
		    
		    //walk list of datapoints and add each one to the message
		    for (di = 0; di < bionet_resource_get_num_datapoints(resource); di++) {
			bionet_datapoint_t * d = bionet_resource_get_datapoint_by_index(resource, di);
			Datapoint_t *asn_datapoint;
			if (NULL == d) {
			    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
				  "Failed to get datapoint %d from Resource %s", di, bionet_resource_get_name(resource));
			}
			
			asn_datapoint = bionet_datapoint_to_asn(d);
			if (asn_datapoint == NULL) {
			    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
				  "send_sync_datapoints(): out of memory!");
			}
			
			r = asn_sequence_add(&resource_rec->resourceDatapoints.list, asn_datapoint);
			if (r != 0) {
			    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
				  "send_sync_datapoints(): error adding Datapoint to Resource: %m");
			}
		    } //for (di = 0; di < bionet_resource_get_num_datapoints(resource); di++) 
		} //for (ri = 0; ri < bionet_node_get_num_resources(node); ri++)
	    } //for (ni = 0; ni < bionet_hab_get_num_nodes(hab); ni++)
	} //for (hi = 0; hi < hab_list->len; hi++)
    } //for (bi = 0; bi < bdm_list->len; bi++)

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
