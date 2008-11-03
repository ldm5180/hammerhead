
//
// Copyright (C) 2008, Regents of the University of Colorado.
//

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
        printf("Error creating Resource\n");
        return;
    }

    r = bionet_node_add_resource(node, resource);
    if (r != 0) {
        printf("Error adding Resource\n");
    }

    // half of the resources start out without a datapoint
    if ((rand() % 2) == 0) {
        printf(
            "    %s %s %s = (no value)\n",
            resource_id,
            bionet_resource_data_type_to_string(data_type),
            bionet_resource_flavor_to_string(flavor)
        );
        return;
    }


    //
    // the other half of the resources get an initial datapoint
    //

    set_random_resource_value(resource);

    datapoint = bionet_resource_get_datapoint_by_index(resource, 0);
    printf(
        "    %s %s %s = %s\n",
        resource_id,
        bionet_resource_data_type_to_string(data_type),
        bionet_resource_flavor_to_string(flavor),
        bionet_datapoint_value_to_string(datapoint)
    );
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

    printf("new Node %s\n", node_id);

    node = bionet_node_new(random_hab, node_id);
    
    // add 0-29 resources
    num_resources = rand() % 30;
    for (i = 0; i < num_resources; i ++) {
        add_resource(node);
    }

    if (bionet_hab_add_node(random_hab, node) != 0) {
	printf("HAB failed to add Node\n");
	return;
    }
    
    if (hab_report_new_node(node) != 0) {
	// printf("error reporting Node to the NAG: %s\n", hab_get_nag_error());
    }
}

