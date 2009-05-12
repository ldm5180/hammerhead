
// Copyright (c) 2008-2009, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"


void handle_gpi_event(LLRP_tSGPIEvent *pGPIEvent) {
    bionet_resource_t *resource;
    char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];

    int gpi_num;
    int new_level;

    gpi_num = pGPIEvent->GPIPortNumber;
    new_level = pGPIEvent->GPIEvent;

    snprintf(resource_id, sizeof(resource_id), "GPI%d", gpi_num);
    resource = bionet_node_get_resource_by_id(reader_node, resource_id);
    if (resource == NULL) {
        g_warning("can't find reader resource %s, ignoring GPI event", resource_id);
        return;
    }

    bionet_resource_set_binary(resource, new_level, NULL);

    hab_report_datapoints(reader_node);

    if ((gpi_num == 1) && (new_level == gpi_polarity)) {
        if (scans_left_to_do == 0) g_timeout_add(gpi_delay, startROSpec, NULL);
        scans_left_to_do = num_scans;
    }
}

