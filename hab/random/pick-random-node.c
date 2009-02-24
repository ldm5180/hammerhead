
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef WINDOWS
    #include <windows.h>
#endif

#include <glib.h>

#include "bionet-util.h"
#include "hardware-abstractor.h"




bionet_node_t *pick_random_node(bionet_hab_t *random_hab) {
    int num_nodes;
    int node_index;

    num_nodes = bionet_hab_get_num_nodes(random_hab);
    if (num_nodes <= 0) return NULL;

    node_index = rand() % num_nodes;

    return bionet_hab_get_node_by_index(random_hab, node_index);
}

