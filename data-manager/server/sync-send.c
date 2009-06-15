
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include <glib.h>

#include <bionet.h>
#include "bionet-util.h"
#include "bdm-util.h"
#include "bionet-data-manager.h"


static int sync_send_metadata(sync_sender_config_t * config, struct timeval * last_sync) {
    GPtrArray * hab_list = NULL;
    struct timeval ts, last_entry_end_time;
    char * hab_type;
    char * hab_id;
    char * node_id;
    char * resource_id;

    //get the most recent entry timestamp in the database. use this as the entry 
    //end time for the query. this allows for the DB to act as the syncronization point
    //instead of creating our own locks.
    db_get_latest_entry_timestamp(&ts);

    if (bionet_split_resource_name(config->resource_name_pattern,
				   &hab_type, &hab_id, &node_id, &resource_id)) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "Failed to split resource name pattern: %s", config->resource_name_pattern);
    }


    hab_list = db_get_metadata(hab_type, hab_id, node_id, resource_id,
			       &config->start_time, &config->end_time,
			       &config->last_entry_end_time, &ts,
			       &last_entry_end_time);
    config->last_entry_end_time = last_entry_end_time;


    //TODO parse the hab list and create the metadata message
    

    return 0;
} /* sync_send_metadata() */


static int sync_send_datapoints(sync_sender_config_t * config, struct timeval * last_sync) {
    GPtrArray * hab_list = NULL;
    struct timeval ts, last_entry_end_time;
    char * hab_type;
    char * hab_id;
    char * node_id;
    char * resource_id;
    BDM_Sync_Datapoints_Message_t message;
    int r, hi;

    //get the most recent entry timestamp in the database. use this as the entry 
    //end time for the query. this allows for the DB to act as the syncronization point
    //instead of creating our own locks.
    db_get_latest_entry_timestamp(&ts);

    if (bionet_split_resource_name(config->resource_name_pattern,
				   &hab_type, &hab_id, &node_id, &resource_id)) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "Failed to split resource name pattern: %s", config->resource_name_pattern);
    }


    hab_list = db_get_metadata(hab_type, hab_id, node_id, resource_id,
			       &config->start_time, &config->end_time,
			       &config->last_entry_end_time, &ts,
			       &last_entry_end_time);
    config->last_entry_end_time = last_entry_end_time;

    //BDM-BP TODO create a sync record for each BDM
    BDMSyncRecord_t * sync_record = (BDMSyncRecord_t *)calloc(1, sizeof(BDMSyncRecord_t));
    if (sync_record) {
	r = asn_sequence_add(&message.list, sync_record);
	if (r != 0) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
		  "sync_send_datapoints(): error adding sync record to BDM Sync Datapoints Message: %m");
	}
    } else {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
	      "sync_send_datapoints(): Failed to malloc sync_record: %m");
    }

    //BDM-BP TODO add BDM-ID to BDM Sync Record

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
		bionet_resource_t * resource = bionet_node_get_resource_by_index(node, ri);
		if (NULL == resource) {
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
			  "Failed to get resource %d from Node %s", ri, bionet_node_get_name(node));
		}

		//walk list of datapoints and add each one to the message
		for (di = 0; di < bionet_resource_get_num_datapoints(resource); di++) {
		    bionet_datapoint_t * datapoint = bionet_resource_get_datapoint_by_index(resource, di);
		    if (NULL == datapoint) {
			g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
			      "Failed to get datapoint %d from Resource %s", di, bionet_resource_get_name(resource));
		    }

		    //BDM-BP TODO add datapoint to message
		}
	    }
	}
    }


    //BDM-BP TODO encode and send message

    return 0;
} /* sync_send_datapoints() */


gpointer sync_thread(gpointer config) {
    struct timeval last_sync = { 0, 0 };
    sync_sender_config_t * cfg = (sync_sender_config_t *)config;

    if (NULL == cfg) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
	      "sync_thread(): NULL config"); 
    }

    while (1) {
	sync_send_metadata(cfg, &last_sync);
	sync_send_datapoints(cfg, &last_sync);
	
	g_usleep(cfg->frequency * G_USEC_PER_SEC);
    }

    return NULL;
} /* sync_thread() */



// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
