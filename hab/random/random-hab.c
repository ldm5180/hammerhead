
//
// Copyright (C) 2008, Regents of the University of Colorado.
//

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef WINDOWS
    #include <windows.h>
#endif

#include <glib.h>

#include "hardware-abstractor.h"

#include "random-hab.h"




// it sets this true in the signal handler and checks it at the top of the
// main loop, so the hab's output can be consistent with the clients'
int should_exit = 0;

om_t output_mode = OM_NORMAL;
bionet_hab_t *hab;




void show_stuff_going_away(void) {
    int i;

    if (output_mode != OM_BIONET_WATCHER) return;

    for (i = 0; i < bionet_hab_get_num_nodes(hab); i ++) {
        bionet_node_t *node = bionet_hab_get_node_by_index(hab, i);
        g_message("lost node: %s", bionet_node_get_name(node));
    }
    g_message("lost hab: %s", bionet_hab_get_name(hab));
}


void signal_handler(int unused) {
    should_exit = 1;
}




void cb_set_resource(bionet_resource_t *resource, bionet_value_t *value) {
    if (output_mode == OM_NORMAL) {
        printf(
            "callback: should set %s to '%s'\n",
            bionet_resource_get_local_name(resource),
            bionet_value_to_str(value)
        );
    }
}




int main (int argc, char *argv[]) {
    int bionet_fd;
    int i;

    int max_delay = 5;
    int min_nodes = 5;

    char *hab_type = HAB_TYPE;
    char *hab_id = NULL;

    bionet_log_context_t log_context = {
        destination: BIONET_LOG_TO_STDOUT,
        log_limit: G_LOG_LEVEL_INFO
    };

    g_log_set_default_handler(bionet_glib_log_handler, &log_context);



    //
    //  Seed the random function
    //

    srand(time(NULL));


    // handle command line arguments
    for (i = 1; i < argc; i ++) {

        if (
            (strcmp(argv[i], "--min-nodes") == 0)
        ) {
            i ++;
            min_nodes = atoi(argv[i]);

        } else if (
            (strcmp(argv[i], "--max-delay") == 0)
        ) {
            i ++;
            max_delay = atoi(argv[i]);

        } else if (
            (strcmp(argv[i], "--id") == 0) ||
            (strcmp(argv[i], "-i") == 0)
        ) {
            i++;
            hab_id = argv[i];

        } else if (
            (strcmp(argv[i], "--output-mode") == 0)
        ) {
            i ++;
            if (strcmp(argv[i], "normal") == 0) output_mode = OM_NORMAL;
            else if (strcmp(argv[i], "bdm-client") == 0) output_mode = OM_BDM_CLIENT;
            else if (strcmp(argv[i], "bionet-watcher") == 0) output_mode = OM_BIONET_WATCHER;
            else {
                fprintf(stderr, "unknown output mode %s\n", argv[i]);
                exit(1);
            }

        } else if (
            (strcmp(argv[i], "--help") == 0) ||
            (strcmp(argv[i], "-h") == 0)
        ) {
            usage(0);

        } else {
            fprintf(stderr, "unknown command-line argument: %s\n", argv[i]);
            exit(1);
        }
    }


    //  
    //  Initializing the hab
    //
    
    hab = bionet_hab_new(hab_type, hab_id);


    //
    //  Connecting to Bionet 
    //

    hab_register_callback_set_resource(cb_set_resource);

    bionet_fd = hab_connect(hab);
    if (bionet_fd < 0) {
        fprintf(stderr, "problem connecting to Bionet, exiting\n");
        return 1;
    }

    if (output_mode == OM_BIONET_WATCHER) {
        g_message("new hab: %s", bionet_hab_get_name(hab));

        signal(SIGTERM, signal_handler);
        signal(SIGINT, signal_handler);
    }


    //
    // give clients a couple of seconds to connect
    // FIXME: racy hack
    //

    {
        time_t start;

        start = time(NULL);

        do {
            time_t now;
            struct timeval timeout;
            fd_set readers;
            int r;

            now = time(NULL);

            if ((now - start) > 2) break;

            timeout.tv_sec = 1;
            timeout.tv_usec = 0;

            FD_ZERO(&readers);
            FD_SET(bionet_fd, &readers);

            r = select(bionet_fd + 1, &readers, NULL, NULL, &timeout);
            if (r < 0) {
                fprintf(stderr, "error from select: %s\n", strerror(errno));
                g_usleep(1000*1000);
                continue;
            }

            hab_read();
        } while(1);
    }


    // 
    // main loop
    // 

    while (1) {
        fd_set readers;
        struct timeval timeout;
        int rnd;
        uint32_t ms_delay;
        int r;

        if (should_exit) {
            sleep(1);  // let the CAL thread emit any last-second events we sent *last* time through the main loop
            show_stuff_going_away();
            exit(0);
        }

	hab_read();

        while (bionet_hab_get_num_nodes(hab) < min_nodes) {
            add_node(hab);
        }

        while (bionet_hab_get_num_nodes(hab) > (2 * min_nodes)) {
            destroy_node(hab);
        }

        rnd = rand() % 100;
        if (rnd < 10) {
            destroy_node(hab);
        } else if (rnd < 20) {
            add_node(hab);
        } else {
            update_node(hab);
        }

        FD_ZERO(&readers);
        FD_SET(bionet_fd, &readers);

        ms_delay = rand() % (max_delay * 1000);
        timeout.tv_sec = ms_delay / 1000;
        timeout.tv_usec = (ms_delay % 1000) * 1000;

        r = select(bionet_fd + 1, &readers, NULL, NULL, &timeout);
        if (
            (r == 0)
            || ((r < 0) && (errno == EINTR))
        ) {
            // timeout or signal
            continue;
        }
        if (r < 0) {
            fprintf(stderr, "error from select: %s\n", strerror(errno));
            g_usleep(1000*1000);
            continue;
        }

        hab_read();
    }

    return 0;
}




void destroy_node(bionet_hab_t* random_hab) {
    bionet_node_t *node;

    node = pick_random_node(random_hab);
    if (node == NULL) return;

    if (output_mode == OM_NORMAL) printf("removing Node %s\n", bionet_node_get_id(node));
    else if (output_mode == OM_BIONET_WATCHER) printf("lost node: %s\n", bionet_node_get_name(node));

    bionet_hab_remove_node_by_id(random_hab, bionet_node_get_id(node));
    hab_report_lost_node(bionet_node_get_id(node));
    bionet_node_free(node);
}

