
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
    struct timeval ts;

    db_get_latest_entry_timestamp(&ts);

    //TODO populate the params correctly
    hab_list = db_get_metadata(NULL, NULL, NULL, NULL,
			       NULL, NULL,
			       NULL, NULL,
			       NULL);

    //TODO parse the hab list and create the metadata message

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
