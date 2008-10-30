
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




#endif //  BIONET_NODE_H

