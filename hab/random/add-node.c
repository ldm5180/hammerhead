
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

#include "node-ids.h"

extern int urandom_fd;

char time_str[64];

static int my_cmp_resource(const void * a_ptr, const void *b_ptr) {
    bionet_resource_t * a = *(bionet_resource_t**)a_ptr; 
    bionet_resource_t * b = *(bionet_resource_t**)b_ptr; 
    int r;

    r = strcmp(bionet_resource_get_id(a), bionet_resource_get_id(b));
    if(0 == r) {
        r = (int)bionet_resource_get_data_type(b) - (int)bionet_resource_get_data_type(a);
    }
    if(0 == r) {
        r = (int)bionet_resource_get_flavor(b) - (int)bionet_resource_get_flavor(a);
    }

    return r;
}

void add_resources(bionet_node_t *node, int num_resources) {
    int r;
    unsigned int rnd;
    int i;

    bionet_resource_t ** resources = malloc(sizeof(bionet_resource_t*) * num_resources);
    if (NULL == resources ) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Out of memory!");
        return;
    }

    for ( i=0; i<num_resources; i++ ) {
        const char *resource_id;
        bionet_resource_flavor_t flavor;
        bionet_resource_data_type_t data_type;

        // Look through the list to ensure this resource is unique
        int unique = 1;
        do {
            int j;
            unique = 1;
            resource_id = get_random_word();
            for ( j=0; j<i; j++ ) {
                if(0 == strcmp(resource_id, bionet_resource_get_id(resources[j]))) {
                    unique=0;
                    break;
                }
            }
        } while(!unique);
        
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

        resources[i] = bionet_resource_new(
            node,
            data_type,
            flavor,
            resource_id
        );
        if (resources[i] == NULL) {
            fprintf(stderr, "Error creating Resource\n");
            return;
        }
    }

    if(sorted_resources) {
        qsort(resources, num_resources, sizeof(bionet_resource_t*), my_cmp_resource);
    }


    // Now add the resources to the node
    for ( i=0; i<num_resources; i++ ) {
        bionet_datapoint_t *datapoint;
        bionet_resource_t * resource;

        const char *resource_id;
        bionet_resource_flavor_t flavor;
        bionet_resource_data_type_t data_type;

        resource  = resources[i];
        resource_id = bionet_resource_get_id(resource);
        flavor = bionet_resource_get_flavor(resource);
        data_type = bionet_resource_get_data_type(resource);

        r = bionet_node_add_resource(node, resource);
        if (r != 0) {
            fprintf(stderr, "Error adding Resource\n");
            continue;
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
        if (output_mode == OM_BDM_CLIENT) {
            g_message(
                "%s,+N,%s,%s %s",
                time_str,
                bionet_resource_get_name(resource),
                bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource))
            );
        }
    }

    free(resources);
}




// 
// Create new random node, report it to Bionet
// 

void add_node(bionet_hab_t* random_hab) {
    bionet_node_t *node;
    char *node_id = NULL;
    int num_resources = 0;
    int i;
    int rnd;


    do {
        int num_node_ids = sizeof(random_node_ids) / sizeof(char*);
	if (sizeof(rnd) != read(urandom_fd, &rnd, sizeof(rnd))) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Error reading from /dev/urandom: %m");
	    return;
	}
        node_id = random_node_ids[abs(rnd) % num_node_ids];
        if (bionet_hab_get_node_by_id(random_hab, node_id) == NULL) break;
    } while(1);

    if (output_mode == OM_NORMAL) printf("new Node %s\n", node_id);

    node = bionet_node_new(random_hab, node_id);

    if (output_mode == OM_BIONET_WATCHER) g_message("new node: %s", bionet_node_get_name(node));
    if (output_mode == OM_BDM_CLIENT) {
        timeval_as_str(NULL, time_str, sizeof(time_str));
        g_message(
            "%s,+N,%s",
            time_str,
            bionet_node_get_name(node)
        );
    }

    // add 0-29 resources
    if (sizeof(rnd) != read(urandom_fd, &rnd, sizeof(rnd))) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Error reading from /dev/urandom: %m");
    } else {
	num_resources = rnd % 30;
	if (num_resources > 0) {
	    if (output_mode == OM_BIONET_WATCHER) g_message("    Resources:");
            add_resources(node, num_resources);
	}
    }

    if (output_mode == OM_BIONET_WATCHER
    ||  output_mode == OM_BDM_CLIENT) 
    {
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

                if (output_mode == OM_BIONET_WATCHER) {
                    g_message(
                        "%s = %s %s %s @ %s",
                        bionet_resource_get_name(resource),
                        bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                        bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                        value_str,
                        bionet_datapoint_timestamp_to_string(dp)
                    );
                } else if (output_mode == OM_BDM_CLIENT) {
                    g_message(
                        "%s,+D,%s,%s %s %s @ %s",
                        time_str,
                        bionet_resource_get_name(resource),
                        bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                        bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                        value_str,
                        bionet_datapoint_timestamp_to_string(dp)
                    );
                }

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

