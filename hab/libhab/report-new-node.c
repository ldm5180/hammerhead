
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <string.h>

#include <glib.h>

#include "hardware-abstractor.h"

#include "libhab-internal.h"


int hab_report_new_node(const bionet_node_t *node) {
#if 0
    int r;
    GSList *i;

    bionet_message_t m;


    if (hab_connect_to_nag() < 0) return -1;

    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_new_node(): tried to report NULL node to nag");
        return -1;
    }
    
    if ( bionet_is_valid_name_component(node->id) != 1 ) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_new_node(): tried to report node with invalid id '%s'", node->id);
        return -1;
    }

    //
    // build the message and send it
    //

    m.type = Bionet_Message_H2N_Node;
    m.body.h2n_node.node = (bionet_node_t *)node;

    r = bionet_nxio_send_message(libhab_nag_nxio, &m);
    if (r < 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_new_node(): error sending new node report to NAG");
        return -1;
    }


    //
    // un-dirty all the resources
    //

    for (i = node->resources; i != NULL; i = i->next) {
        bionet_resource_t *resource = i->data;
        resource->dirty = 0;
    }


    // 
    // read the response from the NAG
    //

    r = libhab_read_ok_from_nag();
    if (r == 0) {
        return 0;
    }

    {
        const char *nag_error;

        nag_error = hab_get_nag_error();
        if (nag_error == NULL) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_new_node(): error reading response from NAG");
        } else {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_new_node(): error from NAG: %s", nag_error);
        }
    }

    return -1;
#endif

    return 0;
}

