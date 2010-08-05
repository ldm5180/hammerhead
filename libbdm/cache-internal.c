
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <glib.h>

#include "bionet-util.h"
#include "libbdm-internal.h"
#include "util/protected.h"

/* BDM Cache: Manage memory for the bionet data tree repersenting all the data
 * that has been sent to bdm-subscriber
 *
 * Keep all metadata seen, annd keep appending new/lost events
 *
 */

void libbdm_cache_add_bdm(bionet_bdm_t *bdm) {
    libbdm_bdms = g_slist_prepend(libbdm_bdms, bdm);
}


void libbdm_cache_remove_bdm(bionet_bdm_t *bdm) {
    libbdm_bdms = g_slist_remove(libbdm_bdms, bdm);
}


void libbdm_cache_add_hab(bionet_hab_t *hab) {
    if (NULL == hab) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "libbdm_cache_add_hab(): NULL hab passed in");
	errno = EINVAL;
	return;
    }

    libbdm_habs = g_slist_prepend(libbdm_habs, hab);
}


void libbdm_cache_remove_hab(bionet_hab_t *hab) {
    libbdm_habs = g_slist_remove(libbdm_habs, hab);
}


void libbdm_cache_add_node(bionet_node_t *node) {
    libbdm_nodes = g_slist_prepend(libbdm_nodes, node);
}


void libbdm_cache_remove_node(bionet_node_t *node) {
    if (NULL == node) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "libbdm_cache_remove_node(): NULL node passed in");
	errno = EINVAL;
	return;
    }

    libbdm_nodes = g_slist_remove(libbdm_nodes, node);

    bionet_hab_t *hab;
    bionet_hab_t *nhab = bionet_node_get_hab(node);
    if (NULL == nhab)
    {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "libbdm_cache_remove_node(): Failed to get hab from node.");
	return;
    }

    hab = bdm_cache_lookup_hab(bionet_hab_get_type(nhab), bionet_hab_get_id(nhab));
    if (hab != nhab) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "cache tried to remove a node which was not completely in cache (%s.%s.%s)", 
	      bionet_hab_get_type(nhab), bionet_hab_get_id(nhab), bionet_node_get_id(node));
        return;
    }

    bionet_hab_remove_node_by_id_and_uid(hab, bionet_node_get_id(node), bionet_node_get_uid(node));
}


void libbdm_cache_add_resource(bionet_resource_t *resource) {
    bionet_node_t *node;

    if (NULL == resource) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "libbdm_cache_add_resource(): NULL resource passed in");
	errno = EINVAL;
	return;
    }
    bionet_node_t * rnode = bionet_resource_get_node(resource);
    node = bdm_cache_lookup_node_uid(bionet_node_get_uid(rnode));

    if(node) {
        if (rnode != node) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                  "cache tried to add a resource which was not created for cache (%s)", 
                  bionet_resource_get_name(resource));
        }
        // else, already in cache
        return;
    }

    if (bionet_node_add_resource(node, resource)) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
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
