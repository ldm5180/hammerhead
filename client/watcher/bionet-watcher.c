
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




#ifdef LINUX
#include <signal.h>

void signal_handler(int signo) {
#if 0
    int hi;

    g_log("", G_LOG_LEVEL_INFO, "cache:");

    for (hi = 0; hi < g_slist_length(bionet_habs); hi ++) {
        int ni;
        bionet_hab_t *hab = g_slist_nth_data(bionet_habs, hi);

        g_log("", G_LOG_LEVEL_INFO, "    %s.%s", hab->type, hab->id);

        for (ni = 0; ni < g_slist_length(hab->nodes); ni ++) {
            int i;
            bionet_node_t *node = g_slist_nth_data(hab->nodes, ni);

            g_log("", G_LOG_LEVEL_INFO, "        %s", node->id);

            for (i = 0; i < g_slist_length(node->resources); i ++) {
                bionet_resource_t *resource = g_slist_nth_data(node->resources, i);

                g_log(
                    "",
                    G_LOG_LEVEL_INFO,
                    "            %s = %s %s %s @ %s",
                    resource->id,
                    bionet_resource_data_type_to_string(resource->data_type),
                    bionet_resource_flavor_to_string(resource->flavor),
                    bionet_resource_value_to_string(resource),
                    bionet_resource_time_to_string_human_readable(resource)
                );
            }

            for (i = 0; i < g_slist_length(node->streams); i ++) {
                bionet_stream_t *stream = g_slist_nth_data(node->streams, i);
                g_log(
                    "", G_LOG_LEVEL_INFO,
                    "            %s %s %s @ %s:%s", 
                    stream->id,
                    stream->type,
                    bionet_stream_direction_to_string(stream->direction),
                    stream->host,
                    bionet_stream_port_to_string(stream->port)
                );
            }

        }
    }
#endif
}

#endif




void cb_datapoint(bionet_datapoint_t *datapoint) {
    g_log(
        "",
        G_LOG_LEVEL_INFO,
        "%s.%s.%s:%s = %s %s %s @ %s",
        datapoint->resource->node->hab->type,
        datapoint->resource->node->hab->id,
        datapoint->resource->node->id,
        datapoint->resource->id,
        bionet_resource_data_type_to_string(datapoint->resource->data_type),
        bionet_resource_flavor_to_string(datapoint->resource->flavor),
        bionet_datapoint_value_to_string(datapoint),
        bionet_datapoint_timestamp_to_string(datapoint)
    );
}


void cb_lost_node(bionet_node_t *node) {
    g_log("", G_LOG_LEVEL_INFO, "lost node: %s.%s.%s", node->hab->type, node->hab->id, node->id);
}


void cb_new_node(bionet_node_t *node) {
    GSList *i;

    g_log("", G_LOG_LEVEL_INFO, "new node: %s.%s.%s", node->hab->type, node->hab->id, node->id);

    if (node->resources) {
        g_log("", G_LOG_LEVEL_INFO, "    Resources:");

        for (i = node->resources; i != NULL; i = i->next) {
            bionet_resource_t *resource = i->data;
            bionet_datapoint_t *datapoint = g_ptr_array_index(resource->datapoints, 0);

            if (datapoint == NULL) {
                g_log(
                    "", G_LOG_LEVEL_INFO,
                    "        %s %s %s (no known value)", 
                    bionet_resource_data_type_to_string(resource->data_type),
                    bionet_resource_flavor_to_string(resource->flavor),
                    resource->id
                );
            } else {
                g_log(
                    "", G_LOG_LEVEL_INFO,
                    "        %s %s %s = %s @ %s", 
                    bionet_resource_data_type_to_string(resource->data_type),
                    bionet_resource_flavor_to_string(resource->flavor),
                    resource->id,
                    bionet_datapoint_value_to_string(datapoint),
                    bionet_datapoint_timestamp_to_string(datapoint)
                );
            }

        }
    }

    if (node->streams) {
        g_log("", G_LOG_LEVEL_INFO, "    Streams:");

        for (i = node->streams; i != NULL; i = i->next) {
            bionet_stream_t *stream = i->data;
            g_log(
                "", G_LOG_LEVEL_INFO,
                "        %s %s %s @ %s:%s", 
                stream->id,
                stream->type,
                bionet_stream_direction_to_string(stream->direction),
                stream->host,
                bionet_stream_port_to_string(stream->port)
            );
        }
    }

}


void cb_lost_hab(bionet_hab_t *hab) {
    g_log("", G_LOG_LEVEL_INFO, "lost hab: %s.%s", hab->type, hab->id);
}


void cb_new_hab(bionet_hab_t *hab) {
    g_log("", G_LOG_LEVEL_INFO, "new hab: %s.%s", hab->type, hab->id);
}




void usage(void) {
    printf("usage: bionet-watcher OPTIONS...\n\
\n\
OPTIONS:\n\
\n\
    --habs HAB-Type.Hab-ID\n\
        Subscribe to a HAB list.\n\
\n\
    --nodes HAB-Type.HAB-ID.Node-ID\n\
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
        g_log("", G_LOG_LEVEL_WARNING, "error connecting to Bionet");
        exit(1);
    }
    g_log("", G_LOG_LEVEL_INFO, "connected to Bionet");


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
        if (strcmp(*argv, "--habs") == 0) {
            argv ++;
            bionet_subscribe_hab_list_by_name(*argv);
            subscribed_to_something = 1;;

        } else if (strcmp(*argv, "--nodes") == 0) {
            argv ++;
            bionet_subscribe_node_list_by_name(*argv);
            subscribed_to_something = 1;;

        } else if ((strcmp(*argv, "-r") == 0) || (strcmp(*argv, "--resource") == 0) || (strcmp(*argv, "--resources") == 0)) {
            argv ++;
            bionet_subscribe_datapoints_by_name(*argv);
            subscribed_to_something = 1;;

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


    // subscriptions are complete, handle all the info we got
    // bionet_handle_queued_nag_messages();


#ifdef LINUX
    signal(SIGUSR1, signal_handler);
#endif


    while (1) {




        while (1) {
            int r;
            fd_set readers;

            FD_ZERO(&readers);
            FD_SET(bionet_fd, &readers);

            r = select(bionet_fd + 1, &readers, NULL, NULL, NULL);

            if ((r < 0) && (errno != EINTR)) {
                g_log("", G_LOG_LEVEL_WARNING, "error from select: %s", strerror(errno));
                g_usleep(1000*1000);
                break;
            }

            bionet_read();
        }
    } 


    // NOT REACHED
    return 0;
}

