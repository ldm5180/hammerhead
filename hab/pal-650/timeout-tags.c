
#include <hardware-abstractor.h>

#include "pal-650-hab.h"


static gboolean node_too_old(gpointer key, gpointer value, gpointer user_data) {
    time_t now = *(time_t *)user_data;
    bionet_node_t *node = (bionet_node_t *)value;
    time_t node_time = *(time_t *)node->user_data;

    if ((now - node_time) > tag_timeout) { 
		return TRUE; 
	}

    return FALSE;
}


void timeout_tags(void) {
    time_t now;

    time(&now);

    g_hash_table_foreach_remove(nodes, node_too_old, &now);
}

