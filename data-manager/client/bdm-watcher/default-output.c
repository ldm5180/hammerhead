
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
   
void cb_datapoint(bionet_datapoint_t *datapoint, bionet_event_t * event, void * usr_data) {
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

    g_message(
        "%s = %s %s %s @ %s",
        bionet_resource_get_name(resource),
        bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
        bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
        value_str,
        bionet_datapoint_timestamp_to_string(datapoint)
    );

    free(value_str);
}


void cb_lost_node(bionet_node_t *node, bionet_event_t * event, void* usr_data) {
    g_message("lost node: %s", bionet_node_get_name(node));
}


void cb_new_node(bionet_node_t *node, bionet_event_t * event, void* usr_data) {
    int i;

    g_message("new node: %s", bionet_node_get_name(node));

    if (bionet_node_get_num_resources(node)) {
        g_message("    Resources:");

        for (i = 0; i < bionet_node_get_num_resources(node); i++) {
            bionet_resource_t *resource = bionet_node_get_resource_by_index(node, i);
	    if (NULL == resource) {
		g_log("", G_LOG_LEVEL_WARNING, "Failed to get resource at index %d from node", i);
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

    if (bionet_node_get_num_streams(node)) {
        g_message("    Streams:");

        for (i = 0; i < bionet_node_get_num_streams(node); i++) {
            bionet_stream_t *stream = bionet_node_get_stream_by_index(node, i);
            if (NULL == stream) {
                g_log("", G_LOG_LEVEL_WARNING, "Failed to get stream at index %d from node", i);
            }

            g_message(
                "        %s %s %s", 
                bionet_stream_get_id(stream),
                bionet_stream_get_type(stream),
                bionet_stream_direction_to_string(bionet_stream_get_direction(stream))
            );
        }
    }
}


void cb_lost_hab(bionet_hab_t *hab, bionet_event_t * event, void* usr_data) {
    g_message("lost hab: %s", bionet_hab_get_name(hab));
}


void cb_new_hab(bionet_hab_t *hab, bionet_event_t * event, void* usr_data) {
    g_message("new hab: %s", bionet_hab_get_name(hab));
    if (bionet_hab_is_secure(hab)) {
        g_message("    %s: security enabled", bionet_hab_get_name(hab));
    }
}

void cb_lost_bdm(bionet_bdm_t *bdm, void* usr_data) {
    g_message("lost bdm: %s", bionet_bdm_get_id(bdm));
}


void cb_new_bdm(bionet_bdm_t *bdm, void* usr_data) {
    g_message("new bdm: %s", bionet_bdm_get_id(bdm));
}

void default_output_register_callbacks(void) {
    // register callbacks
    bdm_register_callback_new_bdm(cb_new_bdm, NULL);
    bdm_register_callback_lost_bdm(cb_lost_bdm, NULL);

    bdm_register_callback_new_hab(cb_new_hab, NULL);
    bdm_register_callback_lost_hab(cb_lost_hab, NULL);

    bdm_register_callback_new_node(cb_new_node, NULL);
    bdm_register_callback_lost_node(cb_lost_node, NULL);
    bdm_register_callback_datapoint(cb_datapoint, NULL);
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
