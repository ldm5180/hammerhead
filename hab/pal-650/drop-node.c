
#include <stdlib.h>

#include <hardware-abstractor.h>

#include "pal-650-hab.h"


void drop_node(gpointer data) {
    bionet_node_t *node = (bionet_node_t *)data;

    g_message("lost node '%s'", node->id);

    hab_report_lost_node(node->id);

    free(node->user_data);
    node->user_data = NULL;

    bionet_node_free(node);
}

