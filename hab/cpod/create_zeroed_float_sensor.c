
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "cpod.h"

int create_zeroed_float_sensor(bionet_node_t* node, char* id) {
	bionet_resource_t* resource;
	int r;

	resource = bionet_resource_new(node,
			BIONET_RESOURCE_DATA_TYPE_FLOAT,
			BIONET_RESOURCE_FLAVOR_SENSOR,
			id);
	if (resource == NULL) {
		g_log("", G_LOG_LEVEL_WARNING, "error creating resource %s", id);
		return -1;
	}

	r = bionet_resource_set_float(resource, 0, NULL);
	if (r < 0) {
		g_log("", G_LOG_LEVEL_WARNING, "error setting resource %s", id);
		return -1;
	} 

	r = bionet_node_add_resource(node, resource);
	if (r < 0) {
		g_log("", G_LOG_LEVEL_WARNING, "node unable to add resource %s", id);
		return -1;
	}
	
	return 0;
}

