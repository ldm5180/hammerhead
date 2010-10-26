
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <glib.h>

#include "bdm-client.h"
   
void cb_bdm_datapoint(bionet_datapoint_t *datapoint, bionet_event_t * event, void * usr_data) {
    bionet_value_t * value = bionet_datapoint_get_value(datapoint);
    if (NULL == value) {
	g_log("", G_LOG_LEVEL_WARNING, "Failed to get value from datapoint.");
	return;
    }

    bionet_resource_t * resource = bionet_value_get_resource(value);
    if (NULL == resource) {
	g_log("", G_LOG_LEVEL_WARNING, "Failed to get resource from value.");
	return;
    }

    char * value_str = bionet_value_to_str(value);
    char * time_str = bionet_event_get_timestamp_as_str(event);

    g_message(
        "%s,+D,%s,%s %s %s @ %s,%s",
        time_str,
        bionet_resource_get_name(resource),
        bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
        bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
        value_str,
        bionet_datapoint_timestamp_to_string(datapoint),
        bionet_event_get_bdm_id(event)
    );

    free(value_str);
    free(time_str);
}


void cb_bdm_lost_node(bionet_node_t *node, bionet_event_t * event, void* usr_data) {
    char * time_str = bionet_event_get_timestamp_as_str(event);
    g_message(
        "%s,-N,%s,,%s",
        time_str,
        bionet_node_get_name(node),
        bionet_event_get_bdm_id(event)
    );
    free(time_str);
}


void cb_bdm_new_node(bionet_node_t *node, bionet_event_t * event, void* usr_data) {
    int i;

    char * time_str = bionet_event_get_timestamp_as_str(event);
    g_message(
        "%s,+N,%s,,%s",
        time_str,
        bionet_node_get_name(node),
        bionet_event_get_bdm_id(event)
    );

    for (i = 0; i < bionet_node_get_num_resources(node); i++) {
        bionet_resource_t *resource = bionet_node_get_resource_by_index(node, i);
        if (NULL == resource) {
            g_log("", G_LOG_LEVEL_WARNING, "Failed to get resource at index %d from node", i);
            continue;
        }
        g_message(
            "%s,+R,%s,%s %s,%s",
            time_str,
            bionet_resource_get_name(resource),
            bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
            bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
            bionet_event_get_bdm_id(event)
        );
    }

    if (bionet_node_get_num_streams(node)) {
        for (i = 0; i < bionet_node_get_num_streams(node); i++) {
            bionet_stream_t *stream = bionet_node_get_stream_by_index(node, i);
            if (NULL == stream) {
                g_log("", G_LOG_LEVEL_WARNING, "Failed to get stream at index %d from node", i);
            }

            g_message(
                "%s,+S,%s,%s %s,%s",
                time_str,
                bionet_stream_get_name(stream),
                bionet_stream_get_type(stream),
                bionet_stream_direction_to_string(bionet_stream_get_direction(stream)),
                bionet_event_get_bdm_id(event)
            );
        }
    }
    free(time_str);
}


void cb_bdm_lost_hab(bionet_hab_t *hab, bionet_event_t * event, void* usr_data) {
    char * time_str = bionet_event_get_timestamp_as_str(event);
    g_message(
        "%s,-H,%s,,%s",
        time_str,
        bionet_hab_get_name(hab),
        bionet_event_get_bdm_id(event)
    );
    free(time_str);
}


void cb_bdm_new_hab(bionet_hab_t *hab, bionet_event_t * event, void* usr_data) {
    char * time_str = bionet_event_get_timestamp_as_str(event);
    g_message(
        "%s,+H,%s,%s,%s",
        time_str,
        bionet_hab_get_name(hab),
        bionet_hab_is_secure(hab)?"security enabled":"no security",
        bionet_event_get_bdm_id(event)
    );
    free(time_str);
}

void cb_bdm_lost_bdm(bionet_bdm_t *bdm, void* usr_data) {
    g_message("lost bdm: %s", bionet_bdm_get_id(bdm));
}


void cb_bdm_new_bdm(bionet_bdm_t *bdm, void* usr_data) {
    g_message("new bdm: %s", bionet_bdm_get_id(bdm));
}

void bdm_watcher_output_register_callbacks(void) {
    // register callbacks
    bdm_register_callback_new_bdm(cb_bdm_new_bdm, NULL);
    bdm_register_callback_lost_bdm(cb_bdm_lost_bdm, NULL);

    bdm_register_callback_new_hab(cb_bdm_new_hab, NULL);
    bdm_register_callback_lost_hab(cb_bdm_lost_hab, NULL);

    bdm_register_callback_new_node(cb_bdm_new_node, NULL);
    bdm_register_callback_lost_node(cb_bdm_lost_node, NULL);
    bdm_register_callback_datapoint(cb_bdm_datapoint, NULL);
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
