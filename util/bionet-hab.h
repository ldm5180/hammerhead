
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

int bionet_hab_remove_node_by_id(bionet_hab_t *hab, const char *node_id);




//
//       NAME: bionet_hab_matches_type_and_id()
//
//   FUNCTION: Checks if a HAB matches a HAB-Type and HAB-ID pair.  The
//             wildcard "*" matches any string.
//
//  ARGUMENTS: The HAB to test, and the HAB-Type and HAB-ID to compare to.
//
//    RETURNS: TRUE (1) if the HAB matches, FALSE (0) if not.
//

int bionet_hab_matches_type_and_id(const bionet_hab_t *hab, const char *type, const char *id);




#endif //  BIONET_HAB_H

