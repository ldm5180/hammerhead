
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#ifdef WINDOWS
    #include <winsock2.h>
#endif

#include <glib.h>

#include "bionet.h"
#include "bionet-util.h"




#if defined(LINUX) || defined(MACOSX)
#include <signal.h>

void signal_handler(int signo) {
    int hi;

    g_message("cache:");

    for (hi = 0; hi < bionet_cache_get_num_habs(); hi ++) {
        int ni;
        bionet_hab_t *hab = bionet_cache_get_hab_by_index(hi);

        g_message("    %s", bionet_hab_get_name(hab));

        for (ni = 0; ni < bionet_hab_get_num_nodes(hab); ni ++) {
            int i;
            bionet_node_t *node = bionet_hab_get_node_by_index(hab, ni);

            g_message("        %s", bionet_node_get_id(node));

            for (i = 0; i < bionet_node_get_num_resources(node); i ++) {
                bionet_resource_t *resource = bionet_node_get_resource_by_index(node, i);
                bionet_datapoint_t *d = bionet_resource_get_datapoint_by_index(resource, 0);
		bionet_value_t *value = bionet_datapoint_get_value(d);
                if (d == NULL) {
                    g_message(
                        "            %s (%s %s): (no value)",
                        bionet_resource_get_id(resource),
                        bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                        bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource))
                    );
                } else {
		    char * value_str = bionet_value_to_str(value);
                    g_message(
                        "            %s (%s %s):  %s @ %s",
                        bionet_resource_get_id(resource),
                        bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
                        bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)),
                        value_str,
                        bionet_datapoint_timestamp_to_string(d)
                    );
		    free(value_str);
                }
            }
            for (i = 0; i < bionet_node_get_num_streams(node); i ++) {
                bionet_stream_t *stream = bionet_node_get_stream_by_index(node, i);
                g_message(
                    "            %s %s %s", 
                    bionet_stream_get_id(stream),
                    bionet_stream_get_type(stream),
                    bionet_stream_direction_to_string(bionet_stream_get_direction(stream))
                );
            }
        }
    }
}

#endif /* defined(LINUX) || defined(MACOSX) */




void cb_datapoint(bionet_datapoint_t *datapoint) {
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


void cb_lost_node(bionet_node_t *node) {
    g_message("lost node: %s", bionet_node_get_name(node));
}


void cb_new_node(bionet_node_t *node) {
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


void cb_lost_hab(bionet_hab_t *hab) {
    g_message("lost hab: %s", bionet_hab_get_name(hab));
}


void cb_new_hab(bionet_hab_t *hab) {
    g_message("new hab: %s", bionet_hab_get_name(hab));
}




void usage(void) {
    fprintf(stderr,
	    "'bionet-watcher' displays data from Bionet HABs.\n"
	    "\n"
	    "Usage: bionet-watcher OPTIONS...\n"
	    "\n"
	    " --help                                            Show this help\n"
	    " -e, --require-security                            Require security\n"
	    " -h, --hab, --habs \"HAB-Type.Hab-ID\"               Subscribe to a HAB list.\n"
	    " -n, --node, --nodes \"HAB-Type.HAB-ID.Node-ID\"     Subscribe to a Node list\n"
	    " -r, --resource, --resources \"HAB-Type.HAB-ID.Node-ID:Resource-ID\"\n"
	    "                                                   Subscribe to Resource values.\n"
	    " -s, --security-dir <dir>                          Directory containing security\n"
	    "                                                   certificates\n"
	    " -v, --version                                     Show the version number\n"
	    "\n"
	    "Security can only be required when a security directory has been specified.\n"
	    "  bionet-watcher [--security-dir <dir> [--require-security]]\n"
);
}


int main(int argc, char *argv[]) {
    int bionet_fd;
    char * security_dir = NULL;
    int require_security = 0;
    GSList * hab_list = NULL;
    GSList * node_list = NULL;
    GSList * dp_list = NULL;
    int subscribed_to_something = 0;
    g_log_set_default_handler(bionet_glib_log_handler, NULL);

    //
    // parse command-line arguments
    //
    int i = 0;
    int c;
    while(1) {
	static struct option long_options[] = {
	    {"help", 0, 0, '?'},
	    {"version", 0, 0, 'v'},
	    {"hab", 1, 0, 'h'},
	    {"habs", 1, 0, 'h'},
	    {"node", 1, 0, 'n'},
	    {"nodes", 1, 0, 'n'},
	    {"resource", 1, 0, 'r'},
	    {"resources", 1, 0, 'r'},
	    {"security-dir", 1, 0, 's'},
	    {"require-security", 0, 0, 'e'}, 
	    {0, 0, 0, 0} //this must be last in the list
	};

	c = getopt_long(argc, argv, "?veh:n:r:s:", long_options, &i);
	if ((-1) == c) {
	    break;
	}

	switch (c) {

	case '?':
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

	case 'h':
	    hab_list = g_slist_append(hab_list, optarg);
	    subscribed_to_something = 1;
	    break;

	case 'n':
	    node_list = g_slist_append(node_list, optarg);
	    subscribed_to_something = 1;
	    break;

	case 'r':
	    dp_list = g_slist_append(dp_list, optarg);
	    subscribed_to_something = 1;
	    break;

	case 's':
	    security_dir = optarg;
	    if (NULL == security_dir) {
		g_log("", G_LOG_LEVEL_ERROR, "Failed to assign security directory: %m");
		return (-1);
	    }
	    break;
	    
	case 'v':
	    print_bionet_version(stdout);
	    return 0;

	default:
	    break;
	}
       
    } //while(1)

    if (security_dir) {
	if (bionet_init_security(security_dir, require_security)) {
	    g_log("", G_LOG_LEVEL_WARNING, "Failed to initialize security.");
	}
    }

    // this must happen before anything else
    bionet_fd = bionet_connect();
    if (bionet_fd < 0) {
        fprintf(stderr, "error connecting to Bionet");
        exit(1);
    }


    bionet_register_callback_new_hab(cb_new_hab);
    bionet_register_callback_lost_hab(cb_lost_hab);

    bionet_register_callback_new_node(cb_new_node);
    bionet_register_callback_lost_node(cb_lost_node);

    bionet_register_callback_datapoint(cb_datapoint);

    if (subscribed_to_something) {
	int i;
	for (i = 0; i < g_slist_length(hab_list); i++) {
	    bionet_subscribe_hab_list_by_name(g_slist_nth_data(hab_list, i));
	}
	for (i = 0; i < g_slist_length(node_list); i++) {
	    bionet_subscribe_node_list_by_name(g_slist_nth_data(node_list, i));
	}
	for (i = 0; i < g_slist_length(dp_list); i++) {
	    bionet_subscribe_datapoints_by_name(g_slist_nth_data(dp_list, i));
	}
    } else {
	bionet_subscribe_hab_list_by_name("*.*");
	bionet_subscribe_node_list_by_name("*.*.*");
	bionet_subscribe_datapoints_by_name("*.*.*:*");
    }

    signal(SIGUSR1, signal_handler);


    while (1) {

        while (1) {
            int r;
            fd_set readers;

            FD_ZERO(&readers);
            FD_SET(bionet_fd, &readers);

            r = select(bionet_fd + 1, &readers, NULL, NULL, NULL);

            if ((r < 0) && (errno != EINTR)) {
                fprintf(stderr, "error from select: %s", strerror(errno));
                g_usleep(1000*1000);
                break;
            }

            bionet_read();
        }
    } 


    // NOT REACHED
    return 0;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
