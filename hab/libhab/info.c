
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include "hardware-abstractor.h"

int hab_publish_info(bionet_hab_t * hab, const uint32_t flags) {
    bionet_node_t * node;
    bionet_resource_t * resource;
    char * bionet_version_str = NULL;

    if (flags) {
	node = bionet_hab_get_node_by_id(hab, "bionet-info");
	if (node) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "hab_publish_info(): bionet-info node already exists.");
	    return 1;
	}

	//get a new node 
	node = bionet_node_new(hab, "bionet-info");
	if (NULL == node) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "Failed to create node: bionet-info");
	    return 1;
	}
	bionet_hab_add_node(hab, node);

	//add version
	if (BIONET_INFO_VERSION_FLAG & flags) {
	    resource = bionet_resource_new(node, 
					   BIONET_RESOURCE_DATA_TYPE_STRING, 
					   BIONET_RESOURCE_FLAVOR_SENSOR,
					   "version");
	    if (NULL == resource) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		      "Failed to create resource: bionet-info:version");
	    } else {
		if (bionet_node_add_resource(node, resource)) {
		    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
			  "Failed to add resource %s to node %s",
			  bionet_resource_get_id(resource),
			  bionet_node_get_name(node));
		}

		bionet_version_get(&bionet_version_str);
		bionet_resource_set_str(resource, bionet_version_str, NULL);
	    }
	}

	hab_report_new_node(node);
    } else {
	//remove the node
	const char * node_id = "bionet-info";
	bionet_node_t * node;
	node = bionet_hab_remove_node_by_id(hab, node_id);
	if (hab_report_lost_node(node_id)) {
	    g_warning("Failed to report lost node %s\n", node_id);
	    return 1;
	}

	if (NULL == node) {
	    g_warning("Failed to remove node %s\n", node_id);
	    return 1;
	} else {
	    bionet_node_free(node);
	}
    }

    return 0;
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
