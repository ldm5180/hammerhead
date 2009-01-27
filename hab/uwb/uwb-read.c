
//
// Copyright (C) 2008-2009, Gary Grobe and the Regents of the University of Colorado.
//


#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <glib.h>

#include "hardware-abstractor.h"
#include "uwb.h"

/*
 * Send the RESPONDER message to the UWB.exe program.
 */
int uwb_read(int fd, char* buffer) 
{
	int bytes = 0;
	double x, y;
	bionet_node_t *node;
	bionet_resource_t *resource;
	struct timeval timestamp;

	memset(buffer, '\0', BUFFER_SZ);

	bytes = recvfrom(fd, buffer, BUFFER_SZ, 0, 0, 0);

	if (bytes < 0) {
		g_warning("\tuwb_read(): recvfrom failed, %s", strerror(errno));
		g_error("\tuwb_read(): exiting...");
	}

	buffer[BUFFER_SZ - 1] = '\0';

	// Format sent is really simple
	// <x[double]><y[double]><timestamp[tv.tv_sec]>

	if (sscanf(buffer, "%lf%lf%lu", &x, &y, &(timestamp.tv_sec))) {
        g_warning("Cannot parse the incoming data, continuing...");
        return 0;
	}

	timestamp.tv_usec = 0;

	if (bionet_hab_get_num_nodes(uwb_hab) == 0) {
        // Initialize the first node.
        node = bionet_node_new(uwb_hab, node_id);
		bionet_hab_add_node(uwb_hab, node);
		resource = bionet_resource_new(node,
									   BIONET_RESOURCE_DATA_TYPE_DOUBLE, 
									   BIONET_RESOURCE_FLAVOR_SENSOR,
									   "X");

		if (resource == NULL) {
		    g_error("Failed to create resource: X for id %s", node_id);
			return -1;
		}

		if (bionet_node_add_resource(node, resource)) {
		    g_error("Failed to add X resource to node %s", node_id);
			return -1;
		}

		if (bionet_resource_set_double(resource, x, &timestamp)) {
		    g_error("Failed to set X resource after creation for node %s", node_id);
			return -1;
		}

		resource = bionet_resource_new(node,
									   BIONET_RESOURCE_DATA_TYPE_DOUBLE, 
									   BIONET_RESOURCE_FLAVOR_SENSOR,
									   "Y");

		if (resource == NULL) {
		    g_error("Failed to create resource: Y for id %s", node_id);
			return -1;
		}

		if (bionet_node_add_resource(node, resource)) {
		    g_error("Failed to add Y resource to node %s", node_id);
			return -1;
		}

		if (bionet_resource_set_double(resource, y, &timestamp)) {
		    g_error("Failed to set Y resource after creation for node %s", node_id);
			return -1;
		}
	}

	return bytes;
}

