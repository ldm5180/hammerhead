
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "bdm-stats.h"

unsigned int bdm_stats_interval = 0;
int start_hab = 0;
bionet_hab_t * bdm_hab = NULL;
int ignore_self = 1;

struct timeval dp_ts_accum = { 0, 0 };
struct timeval db_accum = { 0, 0 };

uint32_t num_sync_datapoints = 0;
uint32_t num_bionet_datapoints = 0;
uint32_t num_db_commits = 0;

uint32_t num_bionet_events = 0;
uint32_t num_sync_sent_events = 0;
uint32_t num_sync_recv_events = 0;

bionet_hab_t * start_stat_hab(const char * bdm_id, int *pHab_fd) {
	bionet_hab_t * bdm_hab = bionet_hab_new("Bionet-Data-Manager", bdm_id);
	if (NULL == bdm_hab) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize Statistics HAB.");
	    return NULL;
	} else {
	    *pHab_fd = hab_connect(bdm_hab);
	    if (*pHab_fd == -1) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to connect HAB to Bionet");
		return NULL;
	    }
	    hab_read();
	}

	bionet_node_t * node = bionet_node_new(bdm_hab, "Statistics");
	if (NULL == node) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize Statistics node.");
	    return NULL;
	}


	/* Time Started */
	bionet_resource_t * resource = bionet_resource_new(node, 
							   BIONET_RESOURCE_DATA_TYPE_UINT32,
							   BIONET_RESOURCE_FLAVOR_SENSOR,
							   "Time-Started");
	if (NULL == resource) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize Time Started resource.");
	    return NULL;
	}

	if (bionet_node_add_resource(node, resource)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
		  "Failed to add Time Started resource to Statistics node");
	    return NULL;
	}

	struct timeval tv;
	if (gettimeofday(&tv, NULL)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Failed to get time of day: %m");
	    return NULL;
	} else {
	    if (bionet_resource_set_uint32(resource, (uint32_t)tv.tv_sec, &tv)) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Failed to set Time Started resource value");
	    }
	}


	/* DTN Datapoints */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT32,
				       BIONET_RESOURCE_FLAVOR_SENSOR,
				       "Number-of-Datapoints-over-DTN-Recorded");
	if (NULL == resource) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize Num DTN Datapoints resource.");
	    return NULL;
	}

	if (bionet_node_add_resource(node, resource)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "Failed to add Num DTN Datapoints resource to Statistics node");
	    return NULL;
	}

	if (bionet_resource_set_uint32(resource, num_sync_datapoints, &tv)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Failed to set Num DTN Datapoints resource value");
	}


	/* Bionet Datapoints */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT32,
				       BIONET_RESOURCE_FLAVOR_SENSOR,
				       "Number-of-Local-Datapoints-Recorded");
	if (NULL == resource) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize Num Local Datapoints resource.");
	    return NULL;
	}

	if (bionet_node_add_resource(node, resource)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "Failed to add Num Local Datapoints resource to Statistics node");
	    return NULL;
	}

	if (bionet_resource_set_uint32(resource, num_bionet_datapoints, &tv)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Failed to set Num Local Datapoints resource value");
	}

	/* DTN Datapoints Rate */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_FLOAT,
				       BIONET_RESOURCE_FLAVOR_SENSOR,
				       "DTN-Datapoints-Per-Second");
	if (NULL == resource) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize DTN Datapoints Per Second resource.");
	    return NULL;
	}

	if (bionet_node_add_resource(node, resource)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "Failed to add DTN Datapoints Per Second resource to Statistics node");
	    return NULL;
	}

	if (0 == bdm_stats_interval) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "BDM stats interval is 0. Invalid!");
	    return NULL;
	}


	/* Bionet Datapoints Rate */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_FLOAT,
				       BIONET_RESOURCE_FLAVOR_SENSOR,
				       "Local-Datapoints-Per-Second");
	if (NULL == resource) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize Local Datapoints Per Second resource.");
	    return NULL;
	}

	if (bionet_node_add_resource(node, resource)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "Failed to add Local Datapoints Per Second resource to Statistics node");
	    return NULL;
	}

	if (0 == bdm_stats_interval) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "BDM stats interval is 0. Invalid!");
	    return NULL;
	}


	/* Bionet Datapoints Rate */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT32,
				       BIONET_RESOURCE_FLAVOR_SENSOR,
				       "Datapoint-Latency-ms");
	if (NULL == resource) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize Datapoint Latency resource.");
	    return NULL;
	}

	if (bionet_node_add_resource(node, resource)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "Failed to add Datapoint Latency resource to Statistics node");
	    return NULL;
	}


	/* Bionet Datapoints Rate */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT32,
				       BIONET_RESOURCE_FLAVOR_SENSOR,
				       "DB-Write-Latency-ms");
	if (NULL == resource) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize DB Write Latency resource.");
	    return NULL;
	}

	if (bionet_node_add_resource(node, resource)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "Failed to add DB Write Latency resource to Statistics node");
	    return NULL;
	}

        /* DB Commit */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT32,
				       BIONET_RESOURCE_FLAVOR_SENSOR,
				       "Number-of-DB-Commits");
	if (NULL == resource) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize Num DB Commits resource.");
	    return NULL;
	}

	if (bionet_node_add_resource(node, resource)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "Failed to add Num Local Datapoints resource to Statistics node");
	    return NULL;
	}

	if (bionet_resource_set_uint32(resource, num_bionet_datapoints, &tv)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Failed to set Num DB Commits resource value");
	}

	/* Sync-Receive Events */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT32,
				       BIONET_RESOURCE_FLAVOR_SENSOR,
				       "Sync-Received-Events");
	if (NULL == resource) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize Sync-Received-Events");
	    return NULL;
	}

	if (bionet_node_add_resource(node, resource)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "Failed to add Sync-Received-Events resource to Statistics node");
	    return NULL;
	}

	if (bionet_resource_set_uint32(resource, num_sync_recv_events, &tv)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Failed to set Sync-Received-Events resource value");
	}

	/* Sync-Send Events */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT32,
				       BIONET_RESOURCE_FLAVOR_SENSOR,
				       "Sync-Sent-Events");
	if (NULL == resource) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize Sync-Sent-Events");
	    return NULL;
	}

	if (bionet_node_add_resource(node, resource)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "Failed to add Sync-Sent-Events resource to Statistics node");
	    return NULL;
	}

	if (bionet_resource_set_uint32(resource, num_sync_sent_events, &tv)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Failed to set Sync-Sent-Events resource value");
	}


	/* Bionset Events */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT32,
				       BIONET_RESOURCE_FLAVOR_SENSOR,
				       "Recorded-Bionet-Events");
	if (NULL == resource) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize Recorded-Bionet-Events");
	    return NULL;
	}

	if (bionet_node_add_resource(node, resource)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "Failed to add Recorded-Bionet-Events resource to Statistics node");
	    return NULL;
	}

	if (bionet_resource_set_uint32(resource, num_bionet_events, &tv)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Failed to set Recorded-Bionet-Events resource value");
	}



	/* Add node */
	if (bionet_hab_add_node(bdm_hab, node)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
		  "Failed to add Statistics node to BDM HAB");
	    return NULL;
	}

	/* Report node and datapoints */
	hab_report_new_node(node);
	hab_report_datapoints(node);

        return bdm_hab;
}

static int _update_uint32_res(bionet_node_t * node, const char * name, uint32_t val, struct timeval * tv){
    bionet_resource_t * local = bionet_node_get_resource_by_id(node, name);
    if (NULL == local) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "%s resource not found", name);
	return 1;
    } else {
	uint32_t cur;
	struct timeval tvtmp;
	bionet_resource_get_uint32(local, &cur, &tvtmp);
	if (cur != val) {
	    bionet_resource_set_uint32(local, val, tv);
	}
    }

    return 0;
}

gboolean update_stat_hab(gpointer usr_data) {
    uint32_t num_real = num_bionet_datapoints;
    uint32_t local_last = 0;
    uint32_t dtn_last = 0;
    struct timeval tv;
    bionet_hab_t * hab;
    bionet_node_t * node;
    bionet_resource_t * dtn;
    bionet_resource_t * local;
    bionet_resource_t * dtn_rate;
    bionet_resource_t * local_rate;
    bionet_resource_t * dp_latency;
    bionet_resource_t * db_latency;

    if (gettimeofday(&tv, NULL)) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Failed to get time of day.");
    }


    hab = (bionet_hab_t *)usr_data;
    if (NULL == hab) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "NULL HAB passed in.");
	goto ret;
    }

    node = bionet_hab_get_node_by_id(hab, "Statistics");
    if (NULL == node) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "Statistics node not found.");
	goto ret;
    }

    dtn = bionet_node_get_resource_by_id(node, "Number-of-Datapoints-over-DTN-Recorded");
    if (NULL == dtn) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "Num DTN Datapoints resource not found");
	return 1;
    } else {
	uint32_t cur;
	struct timeval tvtmp;
	bionet_resource_get_uint32(dtn, &cur, &tvtmp);
	dtn_last = cur;
	if (cur != num_sync_datapoints) {
	    bionet_resource_set_uint32(dtn, num_sync_datapoints, &tv);
	}
    }

    local = bionet_node_get_resource_by_id(node, "Number-of-Local-Datapoints-Recorded");
    if (NULL == local) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "Num Local Datapoints resource not found");
	return 1;
    } else {
	uint32_t cur;
	struct timeval tvtmp;
	bionet_resource_get_uint32(local, &cur, &tvtmp);
	local_last = cur;
	if (cur != num_real) {
	    bionet_resource_set_uint32(local, num_real, &tv);
	}
    }

    dtn_rate = bionet_node_get_resource_by_id(node, "DTN-Datapoints-Per-Second");
    if (NULL == dtn_rate) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "DTN Datapoints Per Second resource not found");
	return 1;
    } else {
	uint32_t cur;
	if (0 == bdm_stats_interval) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "BDM stats interval is 0. Invalid!");
	    return 1;
	}
	cur = (float)(num_sync_datapoints - dtn_last)/(float)bdm_stats_interval;
	bionet_resource_set_float(dtn_rate, cur, &tv);
    }

    local_rate = bionet_node_get_resource_by_id(node, "Local-Datapoints-Per-Second");
    if (NULL == local_rate) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "Local Datapoints Per Second resource not found");
	return 1;
    } else {
	uint32_t cur;
	if (0 == bdm_stats_interval) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "BDM stats interval is 0. Invalid!");
	    return 1;
	}
	cur = (float)(num_real - local_last)/(float)bdm_stats_interval;
	bionet_resource_set_float(local_rate, cur, &tv);
    }

    dp_latency = bionet_node_get_resource_by_id(node, "Datapoint-Latency-ms");
    if (NULL == local_rate) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "Datapoint Latency resource not found");
	return 1;
    } else {
	if (local_last > 0) {
	    uint32_t latency = (dp_ts_accum.tv_sec * 1000) + (dp_ts_accum.tv_usec/1000);
	    latency /= (num_real/local_last);
	    bionet_resource_set_uint32(dp_latency, latency, &tv);
	}
	dp_ts_accum.tv_sec = 0;
	dp_ts_accum.tv_usec = 0;
    }

    db_latency = bionet_node_get_resource_by_id(node, "DB-Write-Latency-ms");
    if (NULL == local_rate) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "DB Write Latency resource not found");
	return 1;
    } else {
	if (local_last > 0) {
	    uint32_t latency = (db_accum.tv_sec * 1000) + (db_accum.tv_usec/1000);
	    latency /= (num_real/local_last);
	    bionet_resource_set_uint32(db_latency, latency, &tv);
	}
	db_accum.tv_sec = 0;
	db_accum.tv_usec = 0;
    }

    /* DB Commits */
    if(_update_uint32_res(node, "Number-of-DB-Commits", num_db_commits, &tv)) {
	return 1;
    }
    
    /* Events */
    if(_update_uint32_res(node, "Recorded-Bionet-Events", num_bionet_events, &tv)) {
	return 1;
    }
    if(_update_uint32_res(node, "Sync-Sent-Events", num_sync_sent_events, &tv)) {
	return 1;
    }
    if(_update_uint32_res(node, "Sync-Received-Events", num_sync_recv_events, &tv)) {
	return 1;
    }

    hab_report_datapoints(node);

ret:
    return TRUE;
} /* update_stat_hab() */



