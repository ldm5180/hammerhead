
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




// 
// functions for dealing with nodes
//

bionet_node_t* bionet_node_new(const bionet_hab_t *hab, const char* node_id);

int bionet_node_add_resource(bionet_node_t *node, bionet_resource_t *resource);

int bionet_node_get_num_resources(bionet_node_t *node);
bionet_resource_t *bionet_node_get_resource_by_index(const bionet_node_t *node, unsigned int index);
bionet_resource_t *bionet_node_get_resource_by_id(const bionet_node_t *node, const char *resource_id);

int bionet_node_get_num_streams(bionet_node_t *node);
bionet_stream_t *bionet_node_get_stream_by_index(const bionet_node_t *node, unsigned int index);
bionet_stream_t *bionet_node_get_stream_by_id(const bionet_node_t *node, const char *stream_id);

void bionet_node_free(bionet_node_t *node);




//
//       NAME:  bionet_node_matches_id()
//              bionet_node_matches_habtype_habid_nodeid()
//
//   FUNCTION:  Checks if a Node matches a name specification.  The Node
//              name can be specified as just a Node-ID (in which case the
//              Node's HAB-Type and HAB-ID are considered matching no
//              matter what) or as a HAB-Type, HAB-ID, and Node-ID (in
//              which case all must match).  The wildcard "*" matches all
//              strings.
//
//  ARGUMENTS:  The Node to test for match, optionally the HAB-Type and
//              HAB-ID, and the Node-ID.
//

int bionet_node_matches_id(const bionet_node_t *node, const char *id);
int bionet_node_matches_habtype_habid_nodeid(const bionet_node_t *node, const char *hab_type, const char *hab_id, const char *node_id);




#endif //  BIONET_NODE_H

