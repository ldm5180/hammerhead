
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include "cpod.h"


void set_float_resource(bionet_node_t* node, char *id, float value, struct timeval *tv){
	bionet_resource_t* resource;
	
	resource = bionet_node_get_resource_by_id(node, id);
	if (resource == NULL) {
		g_log("", G_LOG_LEVEL_ERROR, "unable to find resource %s", id);
		return;
	}

	bionet_resource_set_float(resource, value, tv);
}

