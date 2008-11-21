
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <bionet.h>

#include "bionet-data-manager.h"




// 
// main() initializes these from the command-line arguments
//

int hab_list_index = 0;
char *hab_list_name_patterns[(2 * BIONET_NAME_COMPONENT_MAX_LEN)];

int node_list_index = 0;
char *node_list_name_patterns[(3 * BIONET_NAME_COMPONENT_MAX_LEN)];

int resource_index = 0;
char *resource_name_patterns[(4 * BIONET_NAME_COMPONENT_MAX_LEN)];




// 
// bionet callbacks
//


static void cb_datapoint(bionet_datapoint_t *datapoint) {
    (void) db_add_datapoint(datapoint);
}


static void cb_lost_node(bionet_node_t *node) {
    g_log("", G_LOG_LEVEL_INFO, "lost node: %s.%s.%s", node->hab->type, node->hab->id, node->id);
}


static void cb_new_node(bionet_node_t *node) {
    g_log("", G_LOG_LEVEL_INFO, "new node: %s.%s.%s", node->hab->type, node->hab->id, node->id);
    (void) db_add_node(node);
}


static void cb_lost_hab(bionet_hab_t *hab) {
    g_log("", G_LOG_LEVEL_INFO, "lost hab: %s.%s", hab->type, hab->id);
}


static void cb_new_hab(bionet_hab_t *hab) {
    g_log("", G_LOG_LEVEL_INFO, "new hab: %s.%s", hab->type, hab->id);
    (void) db_add_hab(hab);
}




static int bionet_readable_handler(GIOChannel *unused, GIOCondition unused2, void *unused3) {
    if (bionet_is_connected()) {
        bionet_read();
        return TRUE;
    }

    // try to re-connect
    g_idle_add(try_to_connect_to_bionet, NULL);
    return FALSE;
}




//
// this is the only function that main() needs to call
// everything else is hidden away in this file
//

int try_to_connect_to_bionet(void *unused) {
    int bionet_fd;
    GIOChannel *ch;


    //
    // these functions are idempotent, so it doesnt hurt to re-call them
    // each time we try to connect to the nag
    //

    bionet_register_callback_new_hab(cb_new_hab);
    bionet_register_callback_lost_hab(cb_lost_hab);

    bionet_register_callback_new_node(cb_new_node);
    bionet_register_callback_lost_node(cb_lost_node);

    bionet_register_callback_datapoint(cb_datapoint);


    bionet_fd = bionet_connect();
    if (bionet_fd < 0) {
        g_warning("error connecting to Bionet");
        // retry in 5 seconds
        g_timeout_add(5 * 1000, try_to_connect_to_bionet, NULL);
        return FALSE;
    }
    g_message("connected to Bionet");

    ch = g_io_channel_unix_new(bionet_fd);
    g_io_add_watch(ch, G_IO_IN, bionet_readable_handler, GINT_TO_POINTER(bionet_fd));


    //
    // subscribe
    //

    if (
        (hab_list_index == 0) &&
        (node_list_index == 0) &&
        (resource_index == 0)
    ) {
        bionet_subscribe_hab_list_by_name("*.*");
        bionet_subscribe_node_list_by_name("*.*.*");
        bionet_subscribe_datapoints_by_name("*.*.*:*");
    } else {
        int i;

        for (i = 0; i < hab_list_index; i ++) {
            bionet_subscribe_hab_list_by_name(hab_list_name_patterns[i]);
        }

        for (i = 0; i < node_list_index; i ++) {
            bionet_subscribe_node_list_by_name(node_list_name_patterns[i]);
        }

        for (i = 0; i < resource_index; i ++) {
            bionet_subscribe_datapoints_by_name(resource_name_patterns[i]);
        }
    }

    return FALSE;
}

