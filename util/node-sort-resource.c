
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "internal.h"
#include "bionet-util.h"


int bionet_cmp_resource(const void * a_ptr, const void *b_ptr) {
    bionet_resource_t * a = (bionet_resource_t*)a_ptr; 
    bionet_resource_t * b = (bionet_resource_t*)b_ptr; 
    int r;

    r = strcmp(a->id, b->id);
    if(0 == r) {
        r = (int)b->data_type - (int)a->data_type;
    }
    if(0 == r) {
        r = (int)b->flavor - (int)a->flavor;
    }

    return r;
}

void bionet_node_sort_resources(bionet_node_t *node,
        int (*cmp)(const void * a, const void * b))
{

    if(cmp == NULL) {
        cmp = bionet_cmp_resource;
    }
    node->resources = g_slist_sort(node->resources, cmp);

}
