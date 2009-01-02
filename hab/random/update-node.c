
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

    printf("updating Resources on Node %s:\n", bionet_node_get_id(node));

    if (0 == bionet_node_get_num_resources(node)) {
        printf("    no Resources, skipping\n");
        return;
    }

    for (i = 0; i < bionet_node_get_num_resources(node); i++) {
	bionet_resource_t *resource = bionet_node_get_resource_by_index(node, i);
        bionet_datapoint_t *datapoint;

        printf(
            "    %s %s %s = ",
            bionet_resource_get_id(resource),
            bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
            bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource))
        );

        // resources are only updated 50% of the time
        if ((rand() % 2) == 0) {
            printf("*** skipped\n");
            continue;
        }

        set_random_resource_value(resource);

        datapoint = bionet_resource_get_datapoint_by_index(resource, 0);
        printf("%s\n", bionet_value_to_str(bionet_datapoint_get_value(datapoint)));
    }

    if (hab_report_datapoints(node)) printf("PROBLEM UPDATING!!!\n");
}

