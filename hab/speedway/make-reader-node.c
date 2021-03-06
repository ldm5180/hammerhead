
// Copyright (c) 2008-2010, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "hardware-abstractor.h"

#include "ltkc.h"
#include "speedway.h"


int make_reader_node(void) {
    bionet_resource_t *resource;
    int i;
    int gpi[4];
    int r;

    reader_node = bionet_node_new(hab, "reader");
    if (reader_node == NULL) {
        g_warning("error making a node for the Speedway Reader");
        return -1;
    }
    bionet_hab_add_node(hab, reader_node);

    r = read_gpis(gpi);
    if (r != 0) return -1;

    for (i = 1; i < 5; i++) {
        char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];

        snprintf(resource_id, sizeof(resource_id), "GPI%d", i);
        resource = bionet_resource_new(
            reader_node,
            BIONET_RESOURCE_DATA_TYPE_BINARY,
            BIONET_RESOURCE_FLAVOR_SENSOR,
            resource_id
        );
        if (resource == NULL) {
            g_warning("error making Resource reader:GPI1");
            return -1;
        }
        bionet_resource_set_binary(resource, gpi[i-1], NULL);
        if (bionet_node_add_resource(reader_node, resource)) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "Failed to add resource %s to node %s",
		  bionet_resource_get_id(resource),
		  bionet_node_get_name(reader_node));
	}
    }

    hab_report_new_node(reader_node);

    return 0;
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
