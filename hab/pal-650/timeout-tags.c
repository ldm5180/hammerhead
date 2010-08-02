
// Copyright (c) 2008-2010, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>

#include <hardware-abstractor.h>

#include "pal-650-hab.h"


void timeout_tags(void) {
    int i;
    time_t now;

    time(&now);

    for (i = 0; i < bionet_hab_get_num_nodes(hab); i++) {
        bionet_node_t *node = bionet_hab_get_node_by_index(hab, i);
        node_data_t *node_data = bionet_node_get_user_data(node);

        if ((now - node_data->time) > tag_timeout) {
            free(node_data);
            bionet_node_set_user_data(node, NULL);
            bionet_hab_remove_node_by_id(hab, bionet_node_get_id(node));
            hab_report_lost_node(node);
            bionet_node_free(node);
            i --;
        }
    }
}

