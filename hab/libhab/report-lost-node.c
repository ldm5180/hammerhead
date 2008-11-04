
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <glib.h>

#include "hardware-abstractor.h"

#include "libhab-internal.h"


int hab_report_lost_node(const char *node_id) {

    //
    // sanity checks
    //

    if (node_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_lost_node(): NULL Node-ID passed in");
        goto fail0;
    }

    if (bionet_hab_get_node_by_id(libhab_this, node_id) != NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_lost_node(): passed-in Node still exists in this HAB");
        goto fail0;
    }

#if 0
    int r;
    bionet_message_t m;


    if (hab_connect_to_nag() < 0) return -1;


    if (!bionet_is_valid_name_component(node_id)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_lost_node(): invalid Node-ID '%s'", node_id);
        return -1;
    }

    m.type = Bionet_Message_H2N_Lost_Node;
    m.body.h2n_lost_node.id = (char *)node_id;
    r = bionet_nxio_send_message(libhab_nag_nxio, &m);
    if (r < 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_lost_node(): error reporting lost node");
        return -1;
    }

    r = libhab_read_ok_from_nag();
    if (r < 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_lost_node(): error reading response from NAG");
        return -1;
    }
#endif

    return 0;

fail0:
    return -1;
}


