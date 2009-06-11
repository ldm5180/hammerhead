
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


static int sync_send_metadata() {
    return 0;
} /* sync_send_metadata() */


static int sync_send_datapoints() {
    return 0;
} /* sync_send_datapoints() */


gpointer sync_thread(gpointer config) {
    sync_sender_config_t * cfg = (sync_sender_config_t *)config;
    if (NULL == cfg) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
	      "sync_thread(): NULL config"); 
    }

    while (1) {
	sync_send_metadata();
	sync_send_datapoints();
	
	g_usleep(cfg->frequency * G_USEC_PER_SEC);
    }

    return NULL;
} /* sync_thread() */



// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
