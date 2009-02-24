
// Copyright (c) 2008-2009, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <arpa/inet.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "hardware-abstractor.h"
#include "bionet-hab.h"
#include "bionet-node.h"
#include "bionet-resource.h"


#define BUFFER_SIZE 1024
#define NUM_UPDATES 5

static char buffer[BUFFER_SIZE];

extern bionet_hab_t *parsec_hab;

// Parsec information comes in the format:
// <id> <temperature[C]> <range[m]>\r\n
// and then is repeated NUM_UPDATES times in one message

void read_parsec(int fd) {
  ssize_t size;
  unsigned int id, scanned, i;
  char char_id[16];
  char *new_message;
  float range;
  float temp;
  struct timeval timestamp;

  bionet_node_t *node;
  bionet_resource_t *resource;

  size = recvfrom(fd, buffer, BUFFER_SIZE, 0, 0, 0);
  gettimeofday(&timestamp, NULL);
  buffer[BUFFER_SIZE - 1] = '\0';
  new_message = &(buffer[0]);

  for (i = 0; i < NUM_UPDATES; i++) {

	scanned = sscanf(new_message, "%u %f %f", &id, &temp, &range);

	if (scanned != 3) {
	  g_warning("Couldn't properly scan message: >%s<, only scanned %d", buffer, scanned);
	  return;
	}

	snprintf(char_id, 16, "%hd", id);

	node = bionet_hab_get_node_by_id(parsec_hab, char_id);
	if (node == NULL) {
	  // Add the node
	  node = bionet_node_new(parsec_hab, char_id);
	  bionet_hab_add_node(parsec_hab, node);
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

	  if (bionet_resource_set_float(resource, range, &timestamp)) {
		g_error("Failed to set Range resource after creation for node %d", id);
		return;
	  }
	
	  resource = bionet_resource_new(node, 
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

	  if (bionet_resource_set_float(resource, temp, &timestamp)) {
		g_error("Failed to set Temperature resource after creation for node %d", id);
		return;
	  }

	  if (hab_report_new_node(node)) {
		g_message(">%s<", bionet_node_get_id(node));
		g_message(">%s<", bionet_hab_get_id(parsec_hab));
		g_error("Failed to report new node %d", id);
	  }

	} else {
	  // Get the node and update

	  node = bionet_hab_get_node_by_id(parsec_hab, char_id);

	  // Range
	  resource = bionet_node_get_resource_by_id(node, "Range");

	  if (resource == NULL) {
		g_error("Failed to get Range resource for node %d", id);
	  }

	  if (bionet_resource_set_float(resource, range, &timestamp)) {
		g_error("Failed to set Range resource for node %d", id);
		return;
	  }

	  // Temp
	  resource = bionet_node_get_resource_by_id(node, "Temperature");

	  if (resource == NULL) {
		g_error("Failed to get Temperature resource for node %d", id);
	  }

	  if (bionet_resource_set_float(resource, temp, &timestamp)) {
		g_error("Failed to set Temperature resource for node %d", id);
		return;
	  }

	  hab_report_datapoints(node);
	}

	new_message = strchr(new_message, '\r');

	if (new_message == NULL) {
	  break;
	}
	// Skip the \r\n sequence and start anew
	new_message += 2;
  }
}
