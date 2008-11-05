
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <string.h>

#include <glib.h>

#include "hardware-abstractor.h"

#include "libhab-internal.h"
#include "bionet-asn.h"


int hab_report_new_node(const bionet_node_t *node) {
    bionet_asn_buffer_t buf;
    int r;


    //
    // sanity checks
    //

    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_new_node(): NULL Node passed in");
        goto fail0;
    }

    if (node->hab != libhab_this) {
        g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "hab_report_new_node(): passed-in Node does not belong to this HAB (it belongs to %s.%s, this hab is %s.%s)",
            node->hab->type,
            node->hab->id,
            libhab_this->type,
            libhab_this->id
        );
        goto fail0;
    }

    if (bionet_hab_get_node_by_id(libhab_this, node->id) != node) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_new_node(): passed-in Node not found in this HAB");
        goto fail0;
    }


    r = bionet_node_to_asnbuf(node, &buf);
    if (r != 0) {
        // an error has already been logged
        goto fail1;
    }

    // publish the message to any connected subscribers
    cal_server.publish(node->id, buf.buf, buf.size);

    // FIXME: cal_server.publish should take the buf
    free(buf.buf);

    return 0;

fail1:
    if (buf.buf != NULL) free(buf.buf);

fail0:
    return -1;
}

