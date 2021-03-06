
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "cpod.h"


void set_float_resource(bionet_node_t* node, char *id, float value, struct timeval *tv){
	bionet_resource_t* resource;
	
	resource = bionet_node_get_resource_by_id(node, id);
	if (resource == NULL) {
		g_log("", G_LOG_LEVEL_ERROR, "unable to find resource %s", id);
		return;
	}

	if (bionet_resource_set_float(resource, value, tv)) {
	    g_log("", G_LOG_LEVEL_WARNING, "set_float_resource(): Failed to set float.");
	}
}

