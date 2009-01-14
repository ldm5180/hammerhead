
#include <errno.h>
#include <stdio.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"


void handle_gpi_event(LLRP_tSGPIEvent *pGPIEvent) {
    bionet_resource_t *resource;
    char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];

    sprintf(resource_id, "GPI%d", pGPIEvent->GPIPortNumber);
    resource = bionet_node_get_resource_by_id(reader_node, resource_id);
    if (resource == NULL) {
        g_warning("can't find reader resource %s, ignoring GPI event", resource_id);
        return;
    }

    bionet_resource_set_binary(resource, pGPIEvent->GPIEvent, NULL);

    hab_report_datapoints(reader_node);
}

