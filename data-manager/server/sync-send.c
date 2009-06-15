
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

    //TODO populate the params correctly
    hab_list = db_get_metadata(hab_type, hab_id, node_id, resource_id,
			       &config->start_time, &config->end_time,
			       &config->last_entry_end_time, &ts,
			       &last_entry_end_time);
    config->last_entry_end_time = last_entry_end_time;

    //TODO parse the hab list and create the metadata message
#if 0
    hab_list = db_get_metadata(const char *hab_type,
			       const char *hab_id,
			       const char *node_id,
			       const char *resource_id,
			       struct timeval *datapoint_start,
			       struct timeval *datapoint_end,
			       struct timeval *entry_start,
			       struct timeval *entry_end,
			       struct timeval *latest_entry);
#endif
    return 0;
} /* sync_send_metadata() */


static int sync_send_datapoints(sync_sender_config_t * config, struct timeval * last_sync) {
    GPtrArray * hab_list = NULL;
    struct timeval ts;

    db_get_latest_entry_timestamp(&ts);

    //TODO populate the params correctly
    hab_list = db_get_resource_datapoints(NULL, NULL, NULL, NULL,
					  NULL, NULL,
					  NULL, NULL,
					  NULL);

    //TODO parse the hab list and create the datapoints message

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
