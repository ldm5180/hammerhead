
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>

#include <glib.h>

#include "hardware-abstractor.h"




void usage(void);




bionet_hab_t *hab;


// it sets this true in the signal handler and checks it at the top of the
// main loop, so the hab's output can be consistent with the clients'
int should_exit = 0;

void signal_handler(int unused) {
    g_message("quittin' time!");
    should_exit = 1;
}




void cb_set_resource(bionet_resource_t *resource, bionet_value_t *value) {
    bionet_resource_set(resource, value, NULL);
    hab_report_datapoints(bionet_resource_get_node(resource));
}




// 
// Create a node with some commandable resources, and report it to Bionet
// 

void add_node(bionet_hab_t *hab) {
    bionet_node_t *node;
    char *node_id = "set-me";

    bionet_resource_data_type_t data_type;


    node = bionet_node_new(hab, node_id);
    if (node == NULL) {
        g_warning("error making the node!");
        exit(1);
    }

    if (bionet_hab_add_node(hab, node) != 0) {
        g_warning("error adding the node to the hab!");
        exit(1);
    }

    for (data_type = BIONET_RESOURCE_DATA_TYPE_MIN; data_type <= BIONET_RESOURCE_DATA_TYPE_MAX; data_type ++) {
        char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];
        bionet_resource_t *resource;

        snprintf(resource_id, BIONET_NAME_COMPONENT_MAX_LEN, "commandable-%s", bionet_resource_data_type_to_string(data_type));
        resource_id[BIONET_NAME_COMPONENT_MAX_LEN - 1] = (char)0;

        resource = bionet_resource_new(node, data_type, BIONET_RESOURCE_FLAVOR_PARAMETER, resource_id);
        if (resource == NULL) {
            g_warning("error making the %s resource!", resource_id);
            exit(1);
        }

        if (bionet_node_add_resource(node, resource) != 0) {
            g_warning("error adding the %s resource to the node!", resource_id);
            exit(1);
        }
    }
    
    if (hab_report_new_node(node) != 0) {
	g_warning("error reporting node!");
        exit(1);
    }
}




int main (int argc, char *argv[]) {
    int bionet_fd;
    int i;

    char *hab_type = "commandable";
    char *hab_id = NULL;

    char * security_dir = NULL;
    int require_security = 0;

    bionet_log_context_t log_context = {
        destination: BIONET_LOG_TO_STDOUT,
        log_limit: G_LOG_LEVEL_INFO
    };

    bionet_log_use_default_handler(&log_context);

    while(1) {
	int c;
	static struct option long_options[] = {
	    {"help", 0, 0, '?'},
	    {"version", 0, 0, 'v'},
	    {"id", 1, 0, 'i'},
	    {"security-dir", 1, 0, 's'},
	    {"require-security", 0, 0, 'e'},
	    {0, 0, 0, 0} //this must be last in the list
	};

	c = getopt_long(argc, argv, "?hvei:s:", long_options, &i);
	if (c == -1) {
	    break;
	}

	switch (c) {
            case '?':
            case 'h':
                usage();
                return 0;

            case 'e':
                if (security_dir) {
                    require_security++;
                } else {
                    usage();
                    return (-1);
                }
                break;

            case 'i':
                hab_id = optarg;
                break;

            case 's':
                security_dir = optarg;
                break;

            case 'v':
                print_bionet_version(stdout);
                return 0;

            default:
                break;
	}
    }


    if (security_dir) {
	if (hab_init_security(security_dir, require_security)) {
	    g_log("", G_LOG_LEVEL_WARNING, "Failed to initialize security.");
	} 
    }


    hab = bionet_hab_new(hab_type, hab_id);

    hab_register_callback_set_resource(cb_set_resource);

    bionet_fd = hab_connect(hab);
    if (bionet_fd < 0) {
        fprintf(stderr, "problem connecting to Bionet, exiting\n");
        return 1;
    }

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);


    add_node(hab);


    // 
    // main loop
    // 
    while (1) {
	hab_read_with_timeout(NULL);

        if (should_exit) {
            hab_disconnect();  // let the CAL thread emit any last-second events we sent *last* time through the main loop
            break;
        }
    }

    return 0;
}

