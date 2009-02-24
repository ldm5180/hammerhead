
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




void add_resource(bionet_node_t *node) {
    bionet_resource_flavor_t flavor;
    bionet_resource_data_type_t data_type;
    bionet_datapoint_t *datapoint;
    const char *resource_id;
    bionet_resource_t *resource;
    int r;

    do {
        resource_id = get_random_word();
        if (bionet_node_get_resource_by_id(node, resource_id) == NULL) break;
    } while(1);

    flavor = rand() % (BIONET_RESOURCE_FLAVOR_MAX + 1);
    data_type = rand() % (BIONET_RESOURCE_DATA_TYPE_MAX + 1);

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
    if ((rand() % 2) == 0) {
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


    do {
        int num_node_ids = sizeof(random_node_ids) / sizeof(char*);
        node_id = random_node_ids[rand() % num_node_ids];
        if (bionet_hab_get_node_by_id(random_hab, node_id) == NULL) break;
    } while(1);

    if (output_mode == OM_NORMAL) printf("new Node %s\n", node_id);

    node = bionet_node_new(random_hab, node_id);

    if (output_mode == OM_BIONET_WATCHER) g_message("new node: %s", bionet_node_get_name(node));

    // add 0-29 resources
    num_resources = rand() % 30;
    if (num_resources > 0) {
        if (output_mode == OM_BIONET_WATCHER) g_message("    Resources:");
        for (i = 0; i < num_resources; i ++) {
            add_resource(node);
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

