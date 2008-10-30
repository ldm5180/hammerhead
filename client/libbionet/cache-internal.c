
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include "libbionet-internal.h"
#include "bionet.h"




void libbionet_cache_add_hab(bionet_hab_t *hab) {
    bionet_habs = g_slist_prepend(bionet_habs, hab);
}


void libbionet_cache_remove_hab(bionet_hab_t *hab) {
    bionet_habs = g_slist_remove(bionet_habs, hab);
}


void libbionet_cache_add_node(bionet_node_t *node) {
    bionet_hab_t *hab;

    hab = bionet_cache_lookup_hab(node->hab->type, node->hab->id);
    if (hab == NULL) {
        hab = bionet_hab_new(node->hab->type, node->hab->id);
        libbionet_cache_add_hab(hab);
    }

    hab->nodes = g_slist_prepend(hab->nodes, node);
}


void libbionet_cache_remove_node(bionet_node_t *node) {
    bionet_hab_t *hab;

    hab = bionet_cache_lookup_hab(node->hab->type, node->hab->id);
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "cache tried to remove a node for a non-existent HAB (%s.%s.%s)", node->hab->type, node->hab->id, node->id);
        return;
    }

    hab->nodes = g_slist_remove(hab->nodes, node);
}


void libbionet_cache_add_resource(bionet_resource_t *resource) {
    bionet_node_t *node;

    node = bionet_cache_lookup_node(resource->node->hab->type, resource->node->hab->id, resource->node->id);
    if (node == NULL) {
        node = bionet_node_new(resource->node->hab, resource->node->id);
        libbionet_cache_add_node(node);
    }

    node->resources = g_slist_prepend(node->resources, resource);
}


