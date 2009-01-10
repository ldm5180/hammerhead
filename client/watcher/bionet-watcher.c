
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
    bionet_resource_t * resource = bionet_value_get_resource(value);

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
    printf("usage: bionet-watcher OPTIONS...\n\
\n\
OPTIONS:\n\
\n\
    --help\n\
        Show this help.\n\
\n\
    -h, --hab, --habs HAB-Type.Hab-ID\n\
        Subscribe to a HAB list.\n\
\n\
    -n, --node, --nodes HAB-Type.HAB-ID.Node-ID\n\
        Subscribe to a Node list.\n\
\n\
    -r, --resource, --resources HAB-Type.HAB-ID.Node-ID:Resource-ID\n\
        Subscribe to Resource values.\n\
\n\
");
}


int main(int argc, char *argv[]) {
    int bionet_fd;
    int subscribed_to_something = 0;


    g_log_set_default_handler(bionet_glib_log_handler, NULL);


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


    //
    // parse command-line arguments
    //

    argv ++;

    for ( ; *argv != NULL; argv ++) {
        if ((strcmp(*argv, "-h") == 0) || (strcmp(*argv, "--hab") == 0) || (strcmp(*argv, "--habs") == 0)) {
            argv ++;
            bionet_subscribe_hab_list_by_name(*argv);
            subscribed_to_something = 1;;

        } else if ((strcmp(*argv, "-n") == 0) || (strcmp(*argv, "--node") == 0) || (strcmp(*argv, "--nodes") == 0)) {
            argv ++;
            bionet_subscribe_node_list_by_name(*argv);
            subscribed_to_something = 1;;

        } else if ((strcmp(*argv, "-r") == 0) || (strcmp(*argv, "--resource") == 0) || (strcmp(*argv, "--resources") == 0)) {
            argv ++;
            bionet_subscribe_datapoints_by_name(*argv);
            subscribed_to_something = 1;;

        } else if (strcmp(*argv, "--help") == 0) {
            usage();
            exit(0);

        } else {
            usage();
            exit(1);
        }
    }

    if (!subscribed_to_something) {
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
