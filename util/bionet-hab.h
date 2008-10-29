
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#ifndef BIONET_HAB_H
#define BIONET_HAB_H


#include <stdint.h>
#include <time.h>

#include <sys/time.h>

#include <glib.h>

#include "bionet-util.h"




struct bionet_hab {
    char *type;
    char *id;

    GSList *nodes;

    void *user_data;
};




// 
// functions for dealing with habs
//

bionet_hab_t *bionet_hab_new(const char *type, const char *id);

bionet_node_t *bionet_hab_get_node_by_id(bionet_hab_t *hab, const char *node_id);

int bionet_hab_get_num_nodes(const bionet_hab_t *hab);
bionet_node_t *bionet_hab_get_node_by_index(bionet_hab_t *hab, unsigned int index);

int bionet_hab_add_node(bionet_hab_t *hab, bionet_node_t *node);




#endif //  BIONET_HAB_H

