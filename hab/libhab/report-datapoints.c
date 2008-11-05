
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/time.h>

#include <glib.h>


#include "hardware-abstractor.h"

#include "libhab-internal.h"




int hab_report_datapoints(const bionet_node_t *node) {
#if 0
    int r;
    GSList *i;
    bionet_message_h2n_resource_update_item_t *ru;
    bionet_message_t m;


    if (hab_connect_to_nag() < 0) return -1;

    m.type = Bionet_Message_H2N_Resource_Update;
    m.body.h2n_resource_update.node_id = node->id;
    m.body.h2n_resource_update.resource_updates = NULL;

    // build the resource-update list
    for (i = node->resources; i != NULL; i = i->next) {
        bionet_resource_t *resource = i->data;

        if (! resource->dirty) {
            continue;
        }

        ru = calloc(1, sizeof(bionet_message_h2n_resource_update_item_t));
        if (ru == NULL) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_resource_update(): out of memory while building H2N-Resource-Update message");
            return -1;
        }

        ru->resource_id = resource->id;
        ru->value = strdup(bionet_resource_value_to_string(resource));
        if (ru->value == NULL) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_resource_update(): out of memory while building H2N-Resource-Update message");
            free(ru);
            return -1;
        }
        ru->timestamp = strdup(bionet_resource_time_to_string(resource));
        if (ru->timestamp == NULL) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_resource_update(): out of memory while building H2N-Resource-Update message");
            free(ru->value);
            free(ru);
            return -1;
        }

        m.body.h2n_resource_update.resource_updates = g_slist_append(m.body.h2n_resource_update.resource_updates, ru);

        resource->dirty = 0;
    }

    if (g_slist_length(m.body.h2n_resource_update.resource_updates) == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_resource_update(): Node '%s' has no dirty resources, nothing to report", node->id);
        return 0;
    }


    r = bionet_nxio_send_message(libhab_nag_nxio, &m);
    if (r < 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_resource_update(): error sending resource update to NAG");
        return -1;
    }

    // free the resource-update list
    while (m.body.h2n_resource_update.resource_updates != NULL) {
        ru = g_slist_nth_data(m.body.h2n_resource_update.resource_updates, 0);
        m.body.h2n_resource_update.resource_updates = g_slist_remove(m.body.h2n_resource_update.resource_updates, ru);

        free(ru->value);
        free(ru->timestamp);
        free(ru);
    }


    // read the response from the NAG
    r = libhab_read_ok_from_nag();
    if (r < 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_resource_update(): error reading response from NAG");
        return -1;
    }
#endif

    return 0;
}


