
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <bionet.h>

#include "bionet-data-manager.h"
#include "bionet-util.h"




// 
// main() initializes these from the command-line arguments
//

int hab_list_index = 0;
char *hab_list_name_patterns[MAX_SUBSCRIPTIONS];

int node_list_index = 0;
char *node_list_name_patterns[MAX_SUBSCRIPTIONS];

int resource_index = 0;
char *resource_name_patterns[MAX_SUBSCRIPTIONS];

char * security_dir = NULL;
int require_security = 0;

sqlite3 * main_db = NULL;

int no_resources = 0;

extern bionet_hab_t * bdm_hab;
extern int start_hab;

// 
// bionet callbacks
//

extern uint32_t num_bionet_datapoints;
extern int ignore_self;

struct timeval dp_ts_accum = { 0, 0 };
struct timeval db_accum = { 0, 0 };

static void cb_datapoint(bionet_datapoint_t *datapoint) {
    struct timeval tv_before_write;
    if (start_hab) {
	gettimeofday(&tv_before_write, NULL);
    }

    (void) db_add_datapoint(main_db, datapoint);

    /* do not normally keep stats on yourself, so return */
    if ((ignore_self) && (start_hab)) {
	const char * bdm_hab_name = bionet_hab_get_name(bdm_hab);
	const char * datapoint_hab_name = bionet_hab_get_name(bionet_datapoint_get_hab(datapoint));
	if (0 == strcmp(bdm_hab_name, datapoint_hab_name)) {
	    return;
	}
    }

    /* record latency */
    if (start_hab) {
	/* calculate latency since the HAB published. */
	struct timeval * dp_ts;
	struct timeval cur_ts;
	struct timeval dp_latency;
	struct timeval db_latency;

	dp_ts = bionet_datapoint_get_timestamp(datapoint);
	gettimeofday(&cur_ts, NULL);
	dp_latency = bionet_timeval_subtract(&cur_ts, dp_ts);
	dp_ts_accum.tv_sec += dp_latency.tv_sec;
	dp_ts_accum.tv_usec += dp_latency.tv_usec;
	if (dp_ts_accum.tv_usec > 1000000) {
	    dp_ts_accum.tv_sec += 1;
	    dp_ts_accum.tv_usec -= 1000000;
	}

	db_latency = bionet_timeval_subtract(&cur_ts, &tv_before_write);
	db_accum.tv_sec += db_latency.tv_sec;
	db_accum.tv_usec += db_latency.tv_usec;
	if (db_accum.tv_usec > 1000000) {
	    db_accum.tv_sec += 1;
	    db_accum.tv_usec -= 1000000;
	}
    }
    
    num_bionet_datapoints++;
}


static void cb_lost_node(bionet_node_t *node) {
    g_log("", G_LOG_LEVEL_INFO, "lost node: %s.%s.%s", 
	  bionet_hab_get_type(bionet_node_get_hab(node)), 
	  bionet_hab_get_id(bionet_node_get_hab(node)), 
	  bionet_node_get_id(node));
}


static void cb_new_node(bionet_node_t *node) {
    g_log("", G_LOG_LEVEL_INFO, "new node: %s.%s.%s", 
	  bionet_hab_get_type(bionet_node_get_hab(node)), 
	  bionet_hab_get_id(bionet_node_get_hab(node)), 
	  bionet_node_get_id(node));
    (void) db_add_node(main_db, node);
}


static void cb_lost_hab(bionet_hab_t *hab) {
    g_log("", G_LOG_LEVEL_INFO, "lost hab: %s.%s", 
	  bionet_hab_get_type(hab), 
	  bionet_hab_get_id(hab));
}


static void cb_new_hab(bionet_hab_t *hab) {
    g_log("", G_LOG_LEVEL_INFO, "new hab: %s.%s", 
	  bionet_hab_get_type(hab), 
	  bionet_hab_get_id(hab));
    bionet_hab_set_recording_bdm(hab, bionet_bdm_get_id(this_bdm));
    (void) db_add_hab(main_db, hab);
}




static int bionet_readable_handler(GIOChannel *unused, GIOCondition unused2, void *unused3) {
    if (bionet_is_connected()) {
        bionet_read();
        return TRUE;
    }

    // try to re-connect
    g_idle_add(try_to_connect_to_bionet, NULL);
    return FALSE;
}




//
// this is the only function that main() needs to call
// everything else is hidden away in this file
//

int try_to_connect_to_bionet(void *unused) {
    int bionet_fd;
    GIOChannel *ch;


    //
    // these functions are idempotent, so it doesnt hurt to re-call them
    // each time we try to connect to the nag
    //

    bionet_register_callback_new_hab(cb_new_hab);
    bionet_register_callback_lost_hab(cb_lost_hab);

    bionet_register_callback_new_node(cb_new_node);
    bionet_register_callback_lost_node(cb_lost_node);

    bionet_register_callback_datapoint(cb_datapoint);


    bionet_fd = bionet_connect();
    if (bionet_fd < 0) {
        g_warning("error connecting to Bionet");
        // retry in 5 seconds
        g_timeout_add(5 * 1000, try_to_connect_to_bionet, NULL);
        return FALSE;
    }
    g_message("connected to Bionet");

    ch = g_io_channel_unix_new(bionet_fd);
    g_io_add_watch(ch, G_IO_IN, bionet_readable_handler, GINT_TO_POINTER(bionet_fd));


    //
    // subscribe
    //

    if (
        (hab_list_index == 0) &&
        (node_list_index == 0) &&
        (resource_index == 0) && 
	(no_resources == 0)
    ) {
        bionet_subscribe_hab_list_by_name("*.*");
        bionet_subscribe_node_list_by_name("*.*.*");
        bionet_subscribe_datapoints_by_name("*.*.*:*");
    } else {
        int i;

        for (i = 0; i < hab_list_index; i ++) {
            bionet_subscribe_hab_list_by_name(hab_list_name_patterns[i]);
        }

        for (i = 0; i < node_list_index; i ++) {
            bionet_subscribe_node_list_by_name(node_list_name_patterns[i]);
        }

        for (i = 0; i < resource_index; i ++) {
            bionet_subscribe_datapoints_by_name(resource_name_patterns[i]);
        }
    }

    return FALSE;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
