
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
//  publish to the nag
//

void update_node(bionet_hab_t* random_hab) {
    bionet_node_t *node;

    GSList* resource_cursor;


    node = pick_random_node(random_hab);
    if (node == NULL) return;

    printf("updating Resources on Node %s:\n", node->id);

    if (node->resources == NULL) {
        printf("    no Resources, skipping\n");
        return;
    }


    for (resource_cursor = node->resources; resource_cursor != NULL; resource_cursor = g_slist_next(resource_cursor)) {
        bionet_resource_t *resource;
        bionet_datapoint_t *datapoint;

        resource = (bionet_resource_t *)resource_cursor->data;

        printf(
            "    %s %s %s = ",
            resource->id,
            bionet_resource_data_type_to_string(resource->data_type),
            bionet_resource_flavor_to_string(resource->flavor)
        );

        // resources are only updated 50% of the time
        if ((rand() % 2) == 0) {
            printf("*** skipped\n");
            continue;
        }

        set_random_resource_value(resource);

        datapoint = bionet_resource_get_datapoint_by_index(resource, 0);
        printf("%s\n", bionet_datapoint_value_to_string(datapoint));
    }

    if (hab_report_datapoints(node)) printf("PROBLEM UPDATING!!!\n");
}

