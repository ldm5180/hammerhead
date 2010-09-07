
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <bionet.h>

#include "bionet-data-manager.h"
#include "bionet-util.h"
#include "../../util/protected.h"
#include "bdm-db.h"

#include "bdm-stats.h"


// 
// main() initializes these from the command-line arguments
//

int hab_list_index = 0;
gchar ** hab_list_name_patterns = NULL;

int node_list_index = 0;
gchar ** node_list_name_patterns = NULL;

int resource_index = 0;
gchar ** resource_name_patterns = NULL;

char * security_dir = NULL;
int require_security = 0;

int no_resources = 0;

// 
// bionet callbacks
//

static void cb_datapoint(bionet_datapoint_t *datapoint) {
    struct timeval tv_before_write;
    int i;
    if (start_hab) {
	if (gettimeofday(&tv_before_write, NULL)) {
	    g_warning("cb_datapoint: Failed to get time of day: %m");
	}
    }

    dbb_bionet_event_data_t data;
    data.datapoint = datapoint;

    for(i=0; i< bionet_datapoint_get_num_events(datapoint); i++) {
        bionet_event_t * event = bionet_datapoint_get_event_by_index(datapoint, i);
        if(event) {
            (void) dbb_add_event(dbb, DBB_DATAPOINT_EVENT, data, 
                    bionet_bdm_get_id(this_bdm), 
                    bionet_event_get_timestamp(event));
            num_bionet_events++;
        }
    }

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
	if (gettimeofday(&cur_ts, NULL)) {
	    g_warning("cb_datapoint: Failed to get time of day: %m");
	}
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
    int i;
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "lost node: %s.%s.%s", 
	  bionet_hab_get_type(bionet_node_get_hab(node)), 
	  bionet_hab_get_id(bionet_node_get_hab(node)), 
	  bionet_node_get_id(node));

    dbb_bionet_event_data_t data;
    data.node = node;

    for(i=0; i< bionet_node_get_num_events(node); i++) {
        bionet_event_t * event = bionet_node_get_event_by_index(node, i);
        if(event && bionet_event_get_type(event) == BIONET_EVENT_LOST) {
            (void) dbb_add_event(dbb, DBB_LOST_NODE_EVENT, data, 
                    bionet_bdm_get_id(this_bdm), 
                    bionet_event_get_timestamp(event));
            num_bionet_events++;
        }
    }
}


static void cb_new_node(bionet_node_t *node) {
    uint8_t guid[BDM_UUID_LEN];
    int i;

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "new node: %s.%s.%s", 
	  bionet_hab_get_type(bionet_node_get_hab(node)), 
	  bionet_hab_get_id(bionet_node_get_hab(node)), 
	  bionet_node_get_id(node));

    if (bionet_node_get_num_resources(node)) {
        int i;
        g_message("    Resources:");

        for (i = 0; i < bionet_node_get_num_resources(node); i++) {
            bionet_resource_t *resource = bionet_node_get_resource_by_index(node, i);
	    if (NULL == resource) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Failed to get resource at index %d from node", i);
		continue;
	    }
            bionet_datapoint_t *datapoint = bionet_resource_get_datapoint_by_index(resource, 0);

            if (datapoint == NULL) {
                g_message(
                    "        %s %s %s (no known value)", 
                    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                    bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                    bionet_resource_get_id(resource)
                );
            } else {
                char * value_str = bionet_value_to_str(bionet_datapoint_get_value(datapoint));

                g_message(
                    "        %s %s %s = %s @ %s", 
                    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                    bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                    bionet_resource_get_id(resource),
                    value_str,
                    bionet_datapoint_timestamp_to_string(datapoint)
                );

		free(value_str);
            }
        }
    }

    if (db_make_node_guid(node, guid) ) return;
    bionet_node_set_uid(node, guid);

    dbb_bionet_event_data_t data;
    data.node = node;

    for(i=0; i< bionet_node_get_num_events(node); i++) {
        bionet_event_t * event = bionet_node_get_event_by_index(node, i);
        if(event && bionet_event_get_type(event) == BIONET_EVENT_PUBLISHED) {
            (void) dbb_add_event(dbb, DBB_NEW_NODE_EVENT, data, 
                    bionet_bdm_get_id(this_bdm), 
                    bionet_event_get_timestamp(event));
            num_bionet_events++;
        }
    }
}


static void cb_lost_hab(bionet_hab_t *hab) {
    int i;
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "lost hab: %s.%s", 
	  bionet_hab_get_type(hab), 
	  bionet_hab_get_id(hab));

    dbb_bionet_event_data_t data;
    data.hab = hab;


    for(i=0; i< bionet_hab_get_num_events(hab); i++) {
        bionet_event_t * event = bionet_hab_get_event_by_index(hab, i);
        if(event && bionet_event_get_type(event) == BIONET_EVENT_LOST) {
            (void) dbb_add_event(dbb, DBB_LOST_HAB_EVENT, data, 
                    bionet_bdm_get_id(this_bdm), 
                    bionet_event_get_timestamp(event));
            num_bionet_events++;
        }
    }

    if(i == 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "lost hab with no events: %s.%s", 
              bionet_hab_get_type(hab), 
              bionet_hab_get_id(hab));
    }
}


static void cb_new_hab(bionet_hab_t *hab) {
    int i;
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "new hab: %s.%s", 
	  bionet_hab_get_type(hab), 
	  bionet_hab_get_id(hab));

    dbb_bionet_event_data_t data;
    data.hab = hab;

    for(i=0; i< bionet_hab_get_num_events(hab); i++) {
        bionet_event_t * event = bionet_hab_get_event_by_index(hab, i);
        if(event && bionet_event_get_type(event) == BIONET_EVENT_PUBLISHED) {
            (void) dbb_add_event(dbb, DBB_NEW_HAB_EVENT, data, 
                    bionet_bdm_get_id(this_bdm), 
                    bionet_event_get_timestamp(event));
            num_bionet_events++;
        }
    }
}




static int bionet_readable_handler(GIOChannel *unused, GIOCondition unused2, void *unused3) {
    if (bionet_is_connected()) {
        struct timeval tv = {0};
        bionet_read_many(&tv, 0);
        dbb_flush_to_db(dbb);
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

    if (dbb == NULL) {
        dbb = calloc(1, sizeof(bdm_db_batch_t));

    }

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


void disconnect_from_bionet(void *unused) {
    bionet_disconnect();
    return;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
