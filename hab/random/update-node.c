
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef WINDOWS
    #include <windows.h>
#endif

#include <glib.h>

#include "bionet-util.h"
#include "hardware-abstractor.h"

#include "random-hab.h"

extern int urandom_fd;


//  
//  pick a random node
//  change some of its resources
//  publish to bionet
//

void update_node(bionet_hab_t* random_hab) {
    bionet_node_t *node;
    int i;

    node = pick_random_node(random_hab);
    if (node == NULL) return;

    if (output_mode == OM_NORMAL) printf("updating Resources on Node %s:\n", bionet_node_get_id(node));

    if (0 == bionet_node_get_num_resources(node)) {
        if (output_mode == OM_NORMAL) printf("    no Resources, skipping\n");
        return;
    }

    for (i = 0; i < bionet_node_get_num_resources(node); i++) {
	bionet_resource_t *resource = bionet_node_get_resource_by_index(node, i);
        bionet_datapoint_t *datapoint;
        char *val_str;
	uint8_t rnd;
        // resources are only updated 50% of the time

	if (sizeof(rnd) != read(urandom_fd, &rnd, sizeof(rnd))) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Error reading from /dev/urandom: %m");
	    continue;
	} else if ((rnd % 2) == 0) {
	    if (output_mode == OM_NORMAL) {
		g_message(
		    "    %s %s %s = *** skipped!",
		    bionet_resource_get_id(resource),
		    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
		    bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource))
		    );
	    }
	    continue;
	}

        set_random_resource_value(resource);

        datapoint = bionet_resource_get_datapoint_by_index(resource, 0);
        val_str = bionet_value_to_str(bionet_datapoint_get_value(datapoint));

        if (output_mode == OM_NORMAL) {
            g_message(
                "    %s %s %s = %s @ %s",
                bionet_resource_get_id(resource),
                bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                val_str,
                bionet_datapoint_timestamp_to_string(datapoint)
            );
        } else if (output_mode == OM_BIONET_WATCHER) {
            g_message(
                "%s = %s %s %s @ %s",
                bionet_resource_get_name(resource),
                bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                val_str,
                bionet_datapoint_timestamp_to_string(datapoint)
            );
        } else if (output_mode == OM_BDM_CLIENT) {
            char time_str[64];
            g_message(
                "%s,+D,%s,%s %s %s @ %s",
                timeval_as_str(NULL, time_str, sizeof(time_str)),
                bionet_resource_get_name(resource),
                bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                val_str,
                bionet_datapoint_timestamp_to_string(datapoint)
            );
        }

        free(val_str);
    }

    if (hab_report_datapoints(node)) fprintf(stderr, "PROBLEM UPDATING!!!\n");
}

