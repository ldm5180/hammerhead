
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "hardware-abstractor.h"
#include "uwb.h"


void node_remove() {
    bionet_node_t *node;

	if (bionet_hab_get_num_nodes(uwb_hab) == 0) {
        return;
	}

	node = bionet_hab_remove_node_by_id(uwb_hab, node_id);

    if (node == NULL) {
        g_warning("Cannot remove the node from bionet, continuing...");
	}

	if (hab_report_lost_node(node_id)) {
        g_warning("Failed to report lost node %s\n", node_id);
	}

    bionet_node_free(node);

}
