
#include <stdlib.h>

#include <hardware-abstractor.h>

#include "pal-650-hab.h"


void drop_node(gpointer data) {
    bionet_node_t *node = (bionet_node_t *)data;
    node_data_t *node_data = bionet_node_get_user_data(node);

    g_message("lost node '%s'", bionet_node_get_id(node));

    hab_report_lost_node(bionet_node_get_id(node));

    free(node_data);
    bionet_node_set_user_data(node, NULL);

    bionet_node_free(node);
}

