
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#ifndef BDM_LIST_ITERATOR_H
#define BDM_LIST_ITERATOR_H

#include <glib.h>
#include "bionet-util.h"



/**
 * Called by iterator for each bdm
 *
 * @return 0 to continue traversing
 * @return !0 to save place, and exit traversal
 */
typedef int (*bdm_list_bdm_handler_t)(bionet_bdm_t * bdm, void * usr_data);

/**
 * Called by iterator for each hab
 *
 * @return 0 to continue traversing
 * @return !0 to save place, and exit traversal
 */
typedef int (*bdm_list_hab_handler_t)(
        bionet_bdm_t * bdm,
        bionet_hab_t * hab, 
        void * usr_data);

/**
 * Called by iterator for each node
 *
 * @return 0 to continue traversing
 * @return !0 to save place, and exit traversal
 */
typedef int (*bdm_list_node_handler_t)(
        bionet_bdm_t * bdm,
        bionet_node_t * node, 
        void * usr_data);

/**
 * Called by iterator for each resource
 *
 * @return 0 to continue traversing
 * @return !0 to save place, and exit traversal
 */
typedef int (*bdm_list_resource_handler_t)(
        bionet_bdm_t * bdm,
        bionet_resource_t * resource, 
        void * usr_data);

/**
 * Called by iterator for each datapoint
 *
 * @return 0 to continue traversing
 * @return !0 to save place, and exit traversal
 */
typedef int (*bdm_list_datapoint_handler_t)(
        bionet_bdm_t * bdm,
        bionet_datapoint_t * datapoint, 
        void * usr_data);


typedef struct {
    GPtrArray * bdm_list;
    void * usr_data;
    bdm_list_bdm_handler_t bdm_handler;
    bdm_list_hab_handler_t hab_handler;
    bdm_list_node_handler_t node_handler;
    bdm_list_resource_handler_t resource_handler;
    bdm_list_datapoint_handler_t datapoint_handler;

    // State
    int bi;
    int hi;
    int ni;
    int ri;
    int di;
} bdm_list_iterator_t;






/**
 * Create an iterator for a bdm_list as returned by db_get_*_bdm_list
 *
 * Any handler can be NULL.
 *
 * @param [out] iter_buf
 *      Iterator to populate
 *
 */
void bdm_iterator_init(GPtrArray * bdm_list, 
        bdm_list_bdm_handler_t bdm_handler,
        bdm_list_hab_handler_t hab_handler,
        bdm_list_node_handler_t node_handler,
        bdm_list_resource_handler_t resource_handler,
        bdm_list_datapoint_handler_t datapoint_handler,
        void * usr_data,
        bdm_list_iterator_t * iter_buf
        );


/**
 * Begin or continue bdm_list traversal
 *
 * The iterator will do a depth-first traversal of the bdm_list, calling
 * the handlers for each element. If any handler returns non-zero,
 * then the traversal will stop, and this function will return.
 *
 * If called again with the same iterator, traversal will continue by traversing
 * down to the entity who's handler returned non-zero last.
 *
 * i.e: If a node handler returned 1, then when continuing a bdm handler and
 * hab handler will be called before calling the node handler with the same node.
 *
 * @return 0 when traversal complete
 * @return The non-zero value returned by a handler
 */
int bdm_list_traverse(bdm_list_iterator_t * iter);

#endif // BDM_LIST_ITERATOR_H
