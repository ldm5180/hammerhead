// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

#ifdef __WIN32
#include <winsock2.h>
#include <windows.h>
#endif


#include "hardware-abstractor.h"

#include "glib.h"

#include "info-hab.h"
#include "bionet-util.h"




// it sets this true in the signal handler and checks it at the top of the
// main loop, so the hab's output can be consistent with the clients'
int should_exit = 0;

bionet_hab_t *hab;

static void show_stuff_going_away(void);
static void cb_set_resource(bionet_resource_t *resource, bionet_value_t *value);

void show_stuff_going_away(void) {
    int i;

    for (i = 0; i < bionet_hab_get_num_nodes(hab); i ++) {
        bionet_node_t *node = bionet_hab_get_node_by_index(hab, i);
        g_message("lost node: %s", bionet_node_get_name(node));
    }
    g_message("lost hab: %s", bionet_hab_get_name(hab));
}




void cb_set_resource(bionet_resource_t *resource, bionet_value_t *value) {
    char *value_str = bionet_value_to_str(value);

        printf(
            "callback: should set %s to '%s'\n",
            bionet_resource_get_local_name(resource),
            value_str
        );
    
    free(value_str);
}

void add_info_node(bionet_hab_t *hab) {
    bionet_resource_t *resource;
    bionet_node_t *node;
    bionet_value_t *value;
    bionet_datapoint_t *datapoint;
    int r;

    // Add a new node
    node  = bionet_node_new(hab, "info");
    g_message("new node...\n");

    resource = bionet_resource_new(
        node, 
        BIONET_RESOURCE_DATA_TYPE_STRING,
        BIONET_RESOURCE_FLAVOR_SENSOR,
        "version");
    if (resource == NULL) {
        fprintf(stderr, "Error creating Resource\n");
        return;
    }

    r = bionet_node_add_resource(node, resource);
    if (r != 0) {
        fprintf(stderr, "Error adding Resource\n");
    }

    char * string = NULL;
    bionet_version_get(&string);
    value = bionet_value_new_str(resource, string);
    if (NULL == value) {
        fprintf(stderr, "*** out of memory!\n");
        return;
    }

    datapoint = bionet_datapoint_new(resource, value, NULL);
    bionet_resource_add_datapoint(resource, datapoint);

    bionet_hab_add_node(hab, node);

    hab_report_new_node(node);
}


int main(int argc, char * argv[])
{

    int bionet_fd;

    int max_delay = 5;

    char *hab_type = HAB_TYPE;
    char *hab_id = NULL;


#ifdef __WIN32
    int ret;
    WSADATA wsaData;
    ret = WSAStartup(0x0202, &wsaData);
    if ( ret ) {
        printf("WSAStartup() failed with error %d\n", ret); 
        return 1;
    }
#endif

    print_bionet_version(stdout);
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


        g_message("new hab: %s", bionet_hab_get_name(hab));

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
                fprintf(stderr, "error from select\n");
                g_usleep(1000*1000);
                continue;
            }

            hab_read();
        } while(1);
    }



    srand( (unsigned)time( NULL ) );

    add_info_node(hab);


    // 
    // main loop
    // 
    while (1) {
        fd_set readers;
        struct timeval timeout;
        uint32_t ms_delay;
        int r;

        if (should_exit) {
            hab_disconnect();  // let the CAL thread emit any last-second events we sent *last* time through the main loop
            show_stuff_going_away();
            exit(0);
        }

	hab_read();






        FD_ZERO(&readers);
        FD_SET(bionet_fd, &readers);

        ms_delay = rand();
        ms_delay = abs(ms_delay) % (max_delay * 1000);

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

