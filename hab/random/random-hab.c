
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

#include "hardware-abstractor.h"

#include "random-hab.h"




int terse = 0;




void cb_set_resource(bionet_resource_t *resource, bionet_value_t *value) {
    if (!terse) {
        printf(
            "callback: should set %s to '%s'\n",
            bionet_resource_get_local_name(resource),
            bionet_value_to_str(value)
        );
    }
}




int main (int argc, char *argv[]) {
    bionet_hab_t* hab;
    int bionet_fd;
    int i;

    int max_delay = 5;
    int min_nodes = 5;

    char *hab_type = HAB_TYPE;
    char *hab_id = NULL;


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
            (strcmp(argv[i], "--terse") == 0)
        ) {
            terse = 1;

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


    //  
    //  Interacting with the hab:
    //      -make five nodes
    //      -if there are every less than 5 nodes add more until there are 5 nodes
    //      -iterate through, having a high probability of changing resources
    //       and a lower probability of adding/deleting a node
    //

    sleep(5);

    while (1) {
        fd_set readers;
        struct timeval timeout;
        int rnd;
        uint32_t ms_delay;
        int r;

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
            fprintf(stderr, "error from select: %s", strerror(errno));
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

    if (!terse) printf("removing Node %s\n", bionet_node_get_id(node));

    bionet_hab_remove_node_by_id(random_hab, bionet_node_get_id(node));
    hab_report_lost_node(bionet_node_get_id(node));
    bionet_node_free(node);
}

