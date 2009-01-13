
#include <arpa/inet.h>
#include <glib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "hardware-abstractor.h"

#define BUFFER_SIZE 1024

static char buffer[BUFFER_SIZE];

extern bionet_hab_t *parsec_hab;

// Parsec information comes in the format:
// <id> <range[m]> <temperature[C]>

void read_parsec(int fd) {
  ssize_t size;
  unsigned int id, scanned;
  float range;
  float temp;
  struct timeval timestamp;

  bionet_node_t node;
  bionet_resource_t resource;

  size = recvfrom(fd, buffer, BUFFER_SIZE, 0, 0, 0);
  timestamp = gettimeofday(&timestamp, NULL);

  scanned = sscanf(buffer, "%ud %f %f", &id, &range, &temp);

  if (scanned != 3) {
	buffer[BUFFER_SIZE - 1] = '\0';
	g_warning("Couldn't properly scan message sent: %s", buffer);
	return;
  }

  node = bionet_hab_get_node_by_name(parsec_hab, id);
  if (node == NULL) {
	// Add the node
	node = bionet_node_new(parsec_hab, id);
	resource = bionet_resource_new(node, 
								   BIONET_RESOURCE_DATA_TYPE_FLOAT, 
								   BIONET_RESOURCE_FLAVOR_SENSOR, 
								   "Range");

	if (resource == NULL) {
	  g_error("Failed to create Range resource: Range for id %d", id);
	  return;
	}

	if (bionet_node_add_resource(node, resource)) {
	  g_error("Failed to add Range resource to node %d", id);
	  return;
	}

	if (bionet_set_resource_float(resource, range, &timestamp)) {
	  g_error("Failed to set Range resource after creation for node %d", id);
	  return;
	}
	
	bionet_resource_new(node, 
						BIONET_RESOURCE_DATA_TYPE_FLOAT, 
						BIONET_RESOURCE_FLAVOR_SENSOR, 
						"Temperature");

	if (resource == NULL) {
	  g_error("Failed to create Temperature resource: Range for id %d", id);
	  return;
	}

	if (bionet_node_add_resource(node, resource)) {
	  g_error("Failed to add Temperature resource to node %d", id);
	  return;
	}

	if (bionet_set_resource_float(resource, temp, &timestamp)) {
	  g_error("Failed to set Temperature resource after creation for node %d", id);
	  return;
	}

  } else {
	// Get the node and update

	node = bionet_hab_get_node_by_id(parsec_hab, id);

	// Range
	resource = bionet_node_get_resource_by_id(node, "Range");

	if (resource == NULL) {
	  g_error("Failed to get Range resource for node %d", id);
	}

	if (bionet_set_resource_float(resource, range, &timestamp)) {
	  g_error("Failed to set Range resource for node %d", id);
	  return;
	}

	// Temp
	resource = bionet_node_get_resource_by_id(node, "Temperature");

	if (resource == NULL) {
	  g_error("Failed to get Temperature resource for node %d", id);
	}

	if (bionet_set_resource_float(resource, temp, &timestamp)) {
	  g_error("Failed to set Temperature resource for node %d", id);
	  return;
	}
  }
}
