
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <getopt.h>

#ifdef WINDOWS
    #include <winsock2.h>
#endif

#include <glib.h>

#include "bionet.h"
#include "bdm-client.h"
#include "bionet-util.h"
#include "watcher-callbacks.h"

extern om_t output_mode;

extern const char *current_timestamp_string(void);

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

    if (output_mode == OM_NORMAL) {
        g_message(
            "BDM %s = %s %s %s @ %s",
            bionet_resource_get_name(resource),
            bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
            bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
            value_str,
            bionet_datapoint_timestamp_to_string(datapoint)
        );
    } else if (output_mode == OM_TEST_PATTERN) {
        g_message(
            "BDM %s %s %s '%s'",
            bionet_datapoint_timestamp_to_string(datapoint),
            bionet_node_get_id(bionet_resource_get_node(resource)),
            bionet_resource_get_id(resource),
            value_str
        );
    }

    free(value_str);
}


void cb_bdm_lost_node(bionet_node_t *node, bionet_event_t * event, void * usr_data) {
    if (output_mode == OM_NORMAL) {
        g_message("BDM lost node: %s", bionet_node_get_name(node));
    } else if (output_mode == OM_TEST_PATTERN) {
        g_message("BDM %s - %s", 
            current_timestamp_string(),
            bionet_node_get_id(node));
    }
}


void cb_bdm_new_node(bionet_node_t *node, bionet_event_t * event, void * usr_data) {
    int i;

    if (output_mode == OM_NORMAL) {
        g_message("BDM new node: %s", bionet_node_get_name(node));
    } else if (output_mode == OM_TEST_PATTERN) {
        g_message("BDM %s + %s", 
            current_timestamp_string(),
            bionet_node_get_id(node));
    }

    if (bionet_node_get_num_resources(node)) {
        if (output_mode == OM_NORMAL)
            g_message("    BDM Resources:");

        for (i = 0; i < bionet_node_get_num_resources(node); i++) {
            bionet_resource_t *resource = bionet_node_get_resource_by_index(node, i);
	    if (NULL == resource) {
		g_log("", G_LOG_LEVEL_WARNING, "Failed to get resource at index %d from node", i);
		continue;
	    }
            bionet_datapoint_t *datapoint = bionet_resource_get_datapoint_by_index(resource, 0);

            if (datapoint == NULL) {
                if (output_mode == OM_NORMAL) {
                    g_message(
                        "        BDM %s %s %s (no known value)", 
                        bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                        bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                        bionet_resource_get_id(resource)
                    );
                } else if (output_mode == OM_TEST_PATTERN) {
                    g_message(
                        "    BDM %s %s %s ?",
                        bionet_resource_get_id(resource),
                        bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                        bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource))
                    );
                }
            } else {
                char * value_str = bionet_value_to_str(bionet_datapoint_get_value(datapoint));

                if (output_mode == OM_NORMAL) {
                    g_message(
                        "        BDM %s %s %s = %s @ %s", 
                        bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                        bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                        bionet_resource_get_id(resource),
                        value_str,
                        bionet_datapoint_timestamp_to_string(datapoint)
                    );
                } else if (output_mode == OM_TEST_PATTERN) {
                    g_message(
                        "    BDM %s %s %s '%s'",
                        bionet_resource_get_id(resource),
                        bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                        bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                        value_str
                    );
                }

		free(value_str);
            }

        }
    }

    if (bionet_node_get_num_streams(node)) {
        if (output_mode == OM_NORMAL)
            g_message("    BDM Streams:");

        for (i = 0; i < bionet_node_get_num_streams(node); i++) {
            bionet_stream_t *stream = bionet_node_get_stream_by_index(node, i);
            if (NULL == stream) {
                g_log("", G_LOG_LEVEL_WARNING, "Failed to get stream at index %d from node", i);
            }

            if (output_mode == OM_NORMAL) {
                g_message(
                    "        BDM %s %s %s", 
                    bionet_stream_get_id(stream),
                    bionet_stream_get_type(stream),
                    bionet_stream_direction_to_string(bionet_stream_get_direction(stream))
                );
            }
        }
    }
}


void cb_bdm_lost_hab(bionet_hab_t *hab, bionet_event_t * event, void * usr_data) {
    if (output_mode == OM_NORMAL) {
        g_message("BDM lost hab: %s", bionet_hab_get_name(hab));
    }
}


void cb_bdm_new_hab(bionet_hab_t *hab, bionet_event_t * event, void * usr_data) {
    if (output_mode == OM_NORMAL) {
        g_message("BDM new hab: %s", bionet_hab_get_name(hab));
        if (bionet_hab_is_secure(hab)) {
            g_message("    BDM %s: security enabled", bionet_hab_get_name(hab));
        }
    }
}
