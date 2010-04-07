
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


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
 * Read the message from the UWB.exe program.
 */
int uwb_read(int fd, char* buffer) 
{
	int bytes = 0;
	bionet_node_t *node;
	bionet_resource_t *x_resource, *y_resource;
	struct timeval timestamp;

	memset(buffer, '\0', BUFFER_SZ);

	bytes = recvfrom(fd, buffer, BUFFER_SZ, 0, 0, 0);

	if (bytes < 0) {
		g_warning("\tuwb_read(): recvfrom failed, %s", strerror(errno));
		g_error("\tuwb_read(): exiting...");
	}

	buffer[BUFFER_SZ - 1] = '\0';

	// Format sent is really simple
	// <x[double]><y[double]><timestamp[double(?)]>	


	timestamp.tv_sec = *((time_t*)(&buffer[16]));
	timestamp.tv_usec = 0;

        node = bionet_hab_get_node_by_id(uwb_hab, node_id);

	if (node == NULL) {
        // Initialize the first node.
        node = bionet_node_new(uwb_hab, node_id);
		bionet_hab_add_node(uwb_hab, node);
		x_resource = bionet_resource_new(node,
                                                 BIONET_RESOURCE_DATA_TYPE_DOUBLE, 
                                                 BIONET_RESOURCE_FLAVOR_SENSOR,
                                                 "X");

		if (x_resource == NULL) {
		    g_error("Failed to create resource: X for id %s", node_id);
			return -1;
		}

		if (bionet_node_add_resource(node, x_resource)) {
		    g_error("Failed to add X resource to node %s", node_id);
			return -1;
		}

		y_resource = bionet_resource_new(node,
                                                 BIONET_RESOURCE_DATA_TYPE_DOUBLE, 
                                                 BIONET_RESOURCE_FLAVOR_SENSOR,
                                                 "Y");

		if (y_resource == NULL) {
		    g_error("Failed to create resource: Y for id %s", node_id);
			return -1;
		}

		if (bionet_node_add_resource(node, y_resource)) {
		    g_error("Failed to add Y resource to node %s", node_id);
			return -1;
		}

                if(hab_report_new_node(node) != 0) {
                    g_warning("Failed to report new node");
                    bionet_node_free(node);
                    return -1;
                }
	} 
        
	x_resource = bionet_node_get_resource_by_id(node, "X");
        if (x_resource == NULL) {
            g_error("failed to find Resource 'X' in Node '%s'!", node_id);
            return -1;
        }

        y_resource = bionet_node_get_resource_by_id(node, "Y");
        if (y_resource == NULL) {
            g_error("failed to find Resource 'Y' in Node '%s'!", node_id);
            return -1;
        }
        
        if (bionet_resource_set_double(x_resource, *((double*)(&buffer[0])), &timestamp)) {
            g_error("Failed to set Resource X");
            return -1;
        }

        if (bionet_resource_set_double(y_resource, *((double*)(&buffer[8])), &timestamp)) {
            g_error("Failed to set Resource Y");
            return -1;
        }

	if (hab_report_datapoints(node) != 0) {
            g_error("Failed to report datapoints.");
            return -1;
        }

	return bytes;
}

