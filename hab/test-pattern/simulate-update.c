
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <sys/select.h>
#include <sys/types.h> 
#include <sys/socket.h>

#include "test-pattern-hab.h"
#include "bionet-util.h"
#include "hardware-abstractor.h"

extern int loops;
extern int fast;
extern int current_time;
extern int no_node_updates;

extern int simulate_loops;
extern int max_per_sec;
extern struct timeval tv_wait;

extern int s;

void new_node(struct new_node_event_t *event, struct timeval *tv) {
    bionet_node_t *node;
    GSList *cursor;
    struct timeval * pubtv = tv;

    if (current_time) {
	pubtv = NULL;
    }

    node = bionet_node_new(hab, event->id);
    bionet_hab_add_node(hab, node);

    if ((output_mode == OM_BIONET_WATCHER) || (output_mode == OM_NODES_ONLY))
        g_message("new node: %s", bionet_node_get_name(node));
    if ((output_mode == OM_BIONET_WATCHER) || (output_mode == OM_NODES_ONLY))
        g_message("    Resources:");

    for (cursor = event->resources; cursor != NULL; cursor = cursor->next) {
        struct resource_info_t* res_info;
        bionet_resource_t *resource;

        res_info = (struct resource_info_t*)cursor->data;

        resource = bionet_resource_new(node, res_info->data_type, res_info->flavor, res_info->id);
        if (res_info->has_value == TRUE) {
            bionet_value_t *value;
            bionet_datapoint_t *dp;

            value = str_to_value(resource, res_info->data_type, (char*)res_info->value);
            dp = bionet_datapoint_new(resource, value, pubtv);
            bionet_resource_add_datapoint(resource, dp);

            if ((output_mode == OM_BIONET_WATCHER) || (output_mode == OM_NODES_ONLY)) {
                g_message(
                    "        %s %s %s = %s @ %s",
                    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                    bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                    bionet_resource_get_id(resource),
                    bionet_value_to_str(value),
                    bionet_datapoint_timestamp_to_string(dp)
                );
            }
        } else if ((output_mode == OM_BIONET_WATCHER) || (output_mode == OM_NODES_ONLY)) {
            g_message(
                "        %s %s %s (no known value)",
                bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                bionet_resource_get_id(resource)
            );
        }

	if (bionet_node_add_resource(node, resource)) {
	    g_warning("Failed to add resource %s to node %s",
		      bionet_resource_get_id(resource),
		      bionet_node_get_name(node));
	}
    }
    
    if ((output_mode == OM_BIONET_WATCHER) || (output_mode == OM_RESOURCES_ONLY) || (output_mode == OM_BDM_CLIENT)) {
        for (cursor = event->resources; cursor != NULL; cursor = cursor->next) {
            struct resource_info_t* res_info;
            bionet_resource_t *resource;
            bionet_datapoint_t *dp;

            res_info = (struct resource_info_t*)cursor->data;
            resource = bionet_node_get_resource_by_id(node, res_info->id);
            dp = bionet_resource_get_datapoint_by_index(resource, 0);

            if (dp != NULL) {
                if (output_mode == OM_BDM_CLIENT) {
                    g_message(
                        "%s,%s,%s",
                        bionet_datapoint_timestamp_to_string(dp),
                        bionet_resource_get_name(resource),
                        bionet_value_to_str(bionet_datapoint_get_value(dp))
                    );
                } else {
                    g_message(
                        "%s = %s %s %s @ %s",
                        bionet_resource_get_name(resource),
                        bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                        bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                        bionet_value_to_str(bionet_datapoint_get_value(dp)),
                        bionet_datapoint_timestamp_to_string(dp)
                    );
                }
            }
        }         
    }
    
    hab_read();
    hab_report_new_node(node);
}



void update(struct datapoint_event_t *event, struct timeval *tv) {

    bionet_node_t *node;
    bionet_resource_t *resource;
    bionet_value_t *value;
    bionet_datapoint_t *dp;
    struct timeval *pubtv = tv;

    struct timeval tv_cur;
    struct timeval tv_diff;
    static struct timeval tv_now;
    fd_set dummy;
    
    if (s == -1) {
	s = socket(AF_INET, SOCK_STREAM, 0);
    }
    FD_ZERO(&dummy);
    FD_SET(s, &dummy);

    if (max_per_sec) {
	if (gettimeofday(&tv_now, NULL)) {
	    g_log("", G_LOG_LEVEL_WARNING, "Failed to get time of day.");
	}
    }

    if (current_time) {
	pubtv = NULL;
    }

    node = bionet_hab_get_node_by_id(hab, event->node_id);
    if (node == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "unable to update: node '%s' does not exist", event->node_id);
        return;
    }

    resource = bionet_node_get_resource_by_id(node, event->id);
    if (resource == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "unable to update: resource '%s' does not exist", event->id);
        return;
    }

    bionet_resource_remove_datapoint_by_index(resource, 0);
    value = str_to_value(resource, bionet_resource_get_data_type(resource), (char*)event->value);
    dp = bionet_datapoint_new(resource, value, pubtv);
    bionet_resource_add_datapoint(resource, dp);

    if ((output_mode == OM_BIONET_WATCHER) || (output_mode == OM_RESOURCES_ONLY)) {
        g_message(
            "%s = %s %s %s @ %s",
            bionet_resource_get_name(resource),
            bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
            bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
            bionet_value_to_str(value),
            bionet_datapoint_timestamp_to_string(dp)
        );
    } else if (output_mode == OM_BDM_CLIENT) {
        g_message(
            "%s,%s,%s",
            bionet_datapoint_timestamp_to_string(dp),
            bionet_resource_get_name(resource),
            bionet_value_to_str(bionet_datapoint_get_value(dp))
        );
    }

    hab_read();
    hab_report_datapoints(node);

    if (max_per_sec) {
	if (gettimeofday(&tv_cur, NULL)) {
	    g_log("", G_LOG_LEVEL_WARNING, "Failed to get time of day.");
	}
	tv_diff = bionet_timeval_subtract(&tv_cur, &tv_now);
	if(bionet_timeval_compare(&tv_wait, &tv_diff)) {
	    struct timeval tv_sleep;
	    tv_sleep = bionet_timeval_subtract(&tv_wait, &tv_diff);
	    select(0, 0, 0, &dummy, &tv_sleep);
	}
	tv_now = tv_cur;
    }
}




void simulate_updates(gpointer data, gpointer user_data) {
    struct event_t *event = (struct event_t*)data;
    struct simulate_updates_args_t *args = (struct simulate_updates_args_t*)user_data;

    struct timeval *next, *last;

    fd_set read_fds;
    struct timeval sleep_duration, sleep_until, now;


    next = event->tv;

    if (*(args->tv_ptr) == NULL) {
        last = next;
        *(args->tv_ptr) = next;
    } else {
        last = *(args->tv_ptr);
    }

    if (0 == fast) {
	gettimeofday(&now, NULL);

	// determine the duration to sleep
	sleep_duration = bionet_timeval_subtract(next, last);

	// using "now" and the duration to sleep, calculate the wall time we 
	// should "sleep" until.  Instead of really sleeping, we'll hab_read().
	if(sleep_duration.tv_sec > 0 || (sleep_duration.tv_usec > 0 && sleep_duration.tv_sec == 0)) {
	    sleep_until.tv_usec = (now.tv_usec + sleep_duration.tv_usec) % 1000000;
	    sleep_until.tv_sec  = (now.tv_usec + sleep_duration.tv_usec) / 1000000;
	    sleep_until.tv_sec  = (now.tv_sec  + sleep_duration.tv_sec);
	} else {
	    sleep_until.tv_sec = now.tv_sec;
	    sleep_until.tv_usec = now.tv_usec;
	}

	// "sleep" until sleep_until, except also call hab_read() as needed.
	while(1) {
	    // Recalculate sleep_duration based on how long we've been sleeping so far.
	    gettimeofday(&now, NULL);
	    sleep_duration = bionet_timeval_subtract(&sleep_until, &now);
	    if (!(sleep_duration.tv_sec > 0 || (sleep_duration.tv_usec > 0 && sleep_duration.tv_sec == 0))) {
	        break;
	    }

	    FD_ZERO(&read_fds);
	    FD_SET(args->bionet_fd, &read_fds);
	    if (select(args->bionet_fd + 1, &read_fds, NULL, NULL, &sleep_duration) <= 0) {
	        break;
	    }
	    if(FD_ISSET(args->bionet_fd, &read_fds)) {
	        hab_read();
	    }
	}

	    // set the timeval for the next step
	    *(args->tv_ptr) = next;
    }

    switch (event->type) {
        case NEW_NODE: {
	    if ((0 == no_node_updates) || (0 == simulate_loops)) {
		new_node((struct new_node_event_t*)event->event, next);
	    }
            break;
        }
        case LOST_NODE: {
	    if ((simulate_loops+1 == loops) || (0 == no_node_updates)) {
		struct lost_node_event_t* lost_event;
		bionet_node_t *node;
		
		lost_event = (struct lost_node_event_t*)event->event;
		
		if ((output_mode == OM_BIONET_WATCHER) || (output_mode == OM_NODES_ONLY))
		    g_message("lost node: %s", bionet_node_get_name(bionet_hab_get_node_by_id(hab, lost_event->id)));
		
		node = bionet_hab_remove_node_by_id(hab, lost_event->id);
		bionet_node_free(node);
		
		hab_read();
		hab_report_lost_node(lost_event->id);
	    }
            break;
        }
        case DATAPOINT_UPDATE: {
            update((struct datapoint_event_t*)event->event, next);
            break;
        }
    }

}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
