
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"

/*
 * Create a node and report it to bionet.
 */

int add_node() {
	bionet_node_t *node;

	node = bionet_node_new(hab, "myTestSpeedwayNode");

	if (bionet_hab_add_node(hab, node) != 0) {
		printf("Error: hab failed to add node\n");
		
		return 1;
	}

        // FIXME: add some resources maybe

        hab_report_new_node(node);

	return 0;
}


