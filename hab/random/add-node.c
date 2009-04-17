
// Copyright (c) 2008-2009, Regents of the University of Colorado.
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

#include "node-ids.h"

extern int urandom_fd;


void add_resource(bionet_node_t *node) {
    bionet_resource_flavor_t flavor;
    bionet_resource_data_type_t data_type;
    bionet_datapoint_t *datapoint;
    const char *resource_id;
    bionet_resource_t *resource;
    int r;
    unsigned int rnd;

    do {
        resource_id = get_random_word();
        if (bionet_node_get_resource_by_id(node, resource_id) == NULL) break;
    } while(1);
    
    if (sizeof(rnd) != read(urandom_fd, &rnd, sizeof(rnd))) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Error reading from /dev/urandom: %m");
	return;
    }
    flavor = rnd % (BIONET_RESOURCE_FLAVOR_MAX + 1);

    if (sizeof(rnd) != read(urandom_fd, &rnd, sizeof(rnd))) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Error reading from /dev/urandom: %m");
	return;
    }
    data_type = rnd % (BIONET_RESOURCE_DATA_TYPE_MAX + 1);

    resource = bionet_resource_new(
        node,
        data_type,
        flavor,
        resource_id
    );
    if (resource == NULL) {
        fprintf(stderr, "Error creating Resource\n");
        return;
    }

    r = bionet_node_add_resource(node, resource);
    if (r != 0) {
        fprintf(stderr, "Error adding Resource\n");
    }

    //
    // half of the resources start out without a datapoint
    // the other half of the resources get an initial datapoint
    //
    if (sizeof(rnd) != read(urandom_fd, &rnd, sizeof(rnd))) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Error reading from /dev/urandom: %m");
	return;
    }
    if ((rnd % 2) == 0) {
        if (output_mode == OM_NORMAL) {
            g_message(
                "    %s %s %s = (starts with no value)",
                resource_id,
                bionet_resource_data_type_to_string(data_type),
                bionet_resource_flavor_to_string(flavor)
            );
        } else if (output_mode == OM_BIONET_WATCHER) {
            g_message(
                "        %s %s %s (no known value)",
                bionet_resource_data_type_to_string(data_type),
                bionet_resource_flavor_to_string(flavor),
                resource_id
            );
        }
    } else {
        char *val_str;

        set_random_resource_value(resource);

        datapoint = bionet_resource_get_datapoint_by_index(resource, 0);  // there's only one datapoint
        val_str = bionet_value_to_str(bionet_datapoint_get_value(datapoint));

        if (output_mode == OM_NORMAL) {
            g_message(
                "    %s %s %s = %s @ %s",
                resource_id,
                bionet_resource_data_type_to_string(data_type),
                bionet_resource_flavor_to_string(flavor),
                val_str,
                bionet_datapoint_timestamp_to_string(datapoint)
            );
        } else if (output_mode == OM_BDM_CLIENT) {
            g_message(
                "%s,%s,%s",
                bionet_datapoint_timestamp_to_string(datapoint),
                bionet_resource_get_name(resource),
                val_str
            );
        } else if (output_mode == OM_BIONET_WATCHER) {
            g_message(
                "        %s %s %s = %s @ %s",
                bionet_resource_data_type_to_string(data_type),
                bionet_resource_flavor_to_string(flavor),
                resource_id,
                val_str,
                bionet_datapoint_timestamp_to_string(datapoint)
            );
        }

        free(val_str);
    }
}




// 
// Create new random node, report it to Bionet
// 

void add_node(bionet_hab_t* random_hab) {
    bionet_node_t *node;
    char *node_id;
    int num_resources;
    int i;
    int rnd;


    do {
        int num_node_ids = sizeof(random_node_ids) / sizeof(char*);
	if (sizeof(rnd) != read(urandom_fd, &rnd, sizeof(rnd))) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Error reading from /dev/urandom: %m");
	    break;
	}
        node_id = random_node_ids[abs(rnd) % num_node_ids];
        if (bionet_hab_get_node_by_id(random_hab, node_id) == NULL) break;
    } while(1);

    if (output_mode == OM_NORMAL) printf("new Node %s\n", node_id);

    node = bionet_node_new(random_hab, node_id);

    if (output_mode == OM_BIONET_WATCHER) g_message("new node: %s", bionet_node_get_name(node));

    // add 0-29 resources
    if (sizeof(rnd) != read(urandom_fd, &rnd, sizeof(rnd))) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Error reading from /dev/urandom: %m");
    } else {
	num_resources = rnd % 30;
	if (num_resources > 0) {
	    if (output_mode == OM_BIONET_WATCHER) g_message("    Resources:");
	    for (i = 0; i < num_resources; i ++) {
		add_resource(node);
	    }
	}
    }

    if (output_mode == OM_BIONET_WATCHER) {
        // we have to walk through all the resources again to correctly report the
        // datapoints
        for (i = 0; i < num_resources; i ++) {
            int j;
            bionet_resource_t* resource;

            resource = bionet_node_get_resource_by_index(node, i);
            if (resource == NULL)
                continue;

            for (j = 0; j < bionet_resource_get_num_datapoints(resource); j++) {
                bionet_datapoint_t* dp;
                char *value_str;

                dp = bionet_resource_get_datapoint_by_index(resource, j);
                if (dp == NULL)
                    continue;

                value_str = bionet_value_to_str(bionet_datapoint_get_value(dp));

                g_message(
                    "%s = %s %s %s @ %s",
                    bionet_resource_get_name(resource),
                    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                    bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                    value_str,
                    bionet_datapoint_timestamp_to_string(dp)
                );

                free(value_str);
            }
        }
    }

    if (bionet_hab_add_node(random_hab, node) != 0) {
	fprintf(stderr, "HAB failed to add Node\n");
	return;
    }
    
    if (hab_report_new_node(node) != 0) {
	// printf("error reporting Node to the NAG: %s\n", hab_get_nag_error());
    }
}

