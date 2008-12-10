
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#ifndef BIONET_NODE_H
#define BIONET_NODE_H


#include <stdint.h>
#include <time.h>

#include <sys/time.h>

#include <glib.h>

#include "bionet-util.h"




struct bionet_node {
    const bionet_hab_t *hab;

    char *id;

    GSList *resources;
    GSList *streams;

    void *user_data;
};




/**
 * @file bionet-node.h 
 * Functions for dealing with nodes
 */

bionet_node_t* bionet_node_new(const bionet_hab_t *hab, const char* node_id);

int bionet_node_add_resource(bionet_node_t *node, bionet_resource_t *resource);

int bionet_node_get_num_resources(const bionet_node_t *node);
bionet_resource_t *bionet_node_get_resource_by_index(const bionet_node_t *node, unsigned int index);
bionet_resource_t *bionet_node_get_resource_by_id(const bionet_node_t *node, const char *resource_id);

int bionet_node_get_num_streams(bionet_node_t *node);
bionet_stream_t *bionet_node_get_stream_by_index(const bionet_node_t *node, unsigned int index);
bionet_stream_t *bionet_node_get_stream_by_id(const bionet_node_t *node, const char *stream_id);

void bionet_node_free(bionet_node_t *node);




/**
 * @brief Checks if a Node matches a name specification.
 *
 * @note Also see the bionet_node_matches_habtype_habid_nodeid() function.
 *
 * @param node The Node to check.
 *
 * @param id The Node-ID to check against.  id may be a regular Node-ID, or
 *     it may be the wildcard "*" that matches all Node-IDs.
 *
 * @return FALSE (zero) if the Node does not match the ID, TRUE (non-zero)
 *     if it matches.
 */

int bionet_node_matches_id(const bionet_node_t *node, const char *id);




/**
 * @brief Checks if a Node matches a name specification.
 *
 * @note Also see the bionet_node_matches_id() function.
 *
 * @param node  The Node to check.
 *
 * @param hab_type  The HAB-Type to check against.  It may be a regular
 *     HAB-Type string, or it may be the wildcard "*" that matches all
 *     HAB-Types.
 *
 * @param hab_id  The HAB-ID to check against.  It may be a regular HAB-ID
 *     string, or it may be the wildcard "*" that matches all HAB-IDs.
 *
 * @param node_id  The Node-ID to check against.  It may be a regular
 *     Node-ID string, or it may be the wildcard "*" that matches all
 *     Node-IDs.
 *
 * @return FALSE (zero) if the Node does not match the ID, TRUE (non-zero)
 *     if it matches.
 */

int bionet_node_matches_habtype_habid_nodeid(const bionet_node_t *node, const char *hab_type, const char *hab_id, const char *node_id);




#endif //  BIONET_NODE_H

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
