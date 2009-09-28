
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>

#include "bionet-util.h"
#include "libbionet-internal.h"
#include "bionet.h"


extern GSList *bionet_habs;


void libbionet_cache_add_hab(bionet_hab_t *hab) {
    bionet_habs = g_slist_prepend(bionet_habs, hab);
}


void libbionet_cache_remove_hab(bionet_hab_t *hab) {
    bionet_habs = g_slist_remove(bionet_habs, hab);
}


void libbionet_cache_add_node(bionet_node_t *node) {
    if (NULL == node) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "libbionet_cache_add_node(): NULL node passed in");
	errno = EINVAL;
	return;
    }
    bionet_hab_t *hab;
    bionet_hab_t *nhab = bionet_node_get_hab(node);
    if (NULL == nhab)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "libbionet_cache_add_node(): Failed to get hab from node.");
	return;
    }

    hab = bionet_cache_lookup_hab(bionet_hab_get_type(nhab), bionet_hab_get_id(nhab));
    if (hab == NULL) {
        hab = bionet_hab_new(bionet_hab_get_type(nhab), bionet_hab_get_id(nhab));
        libbionet_cache_add_hab(hab);
    }

    if (bionet_hab_add_node(hab, node)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "libbionet_cache_add_node(): Failed to add node to hab.");
    }
}


void libbionet_cache_remove_node(bionet_node_t *node) {
    if (NULL == node) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "libbionet_cache_remove_node(): NULL node passed in");
	errno = EINVAL;
	return;
    }
    bionet_hab_t *hab;
    bionet_hab_t *nhab = bionet_node_get_hab(node);
    if (NULL == nhab)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "libbionet_cache_add_node(): Failed to get hab from node.");
	return;
    }

    hab = bionet_cache_lookup_hab(bionet_hab_get_type(nhab), bionet_hab_get_id(nhab));
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "cache tried to remove a node for a non-existent HAB (%s.%s.%s)", 
	      bionet_hab_get_type(hab), bionet_hab_get_id(hab), bionet_node_get_id(node));
        return;
    }

    bionet_hab_remove_node_by_id(hab, bionet_node_get_id(node));
}


void libbionet_cache_add_resource(bionet_resource_t *resource) {
    bionet_node_t *node;

    if (NULL == resource) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "libbionet_cache_add_resource(): NULL resource passed in");
	errno = EINVAL;
	return;
    }
    bionet_node_t * rnode = bionet_resource_get_node(resource);
    bionet_hab_t * rhab = bionet_node_get_hab(rnode);
    node = bionet_cache_lookup_node(bionet_hab_get_type(rhab),
				    bionet_hab_get_id(rhab),
				    bionet_node_get_id(rnode));
    if (NULL == node) {
	node = bionet_node_new(rhab, bionet_node_get_id(rnode));
	libbionet_cache_add_node(node);
    }

    if (bionet_node_add_resource(node, resource)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "Failed to add resource %s to node %s",
	      bionet_resource_get_id(resource),
	      bionet_node_get_name(node));
    }
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
