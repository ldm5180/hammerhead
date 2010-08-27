

// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include "bionet-util.h"
#include "protected.h"
#include <glib.h>
#include <string.h>

#include "bdm-list-iterator.h"

void bdm_iterator_init(GPtrArray * bdm_list, 
        bdm_list_bdm_handler_t bdm_handler,
        bdm_list_hab_handler_t hab_handler,
        bdm_list_node_handler_t node_handler,
        bdm_list_resource_handler_t resource_handler,
        bdm_list_datapoint_handler_t datapoint_handler,
        void * usr_data,
        bdm_list_iterator_t * iter
        )
{
    memset(iter, 0, sizeof(bdm_list_iterator_t));

    iter->bdm_list = bdm_list;
    iter->usr_data = usr_data;

    iter->bdm_handler = bdm_handler;
    iter->hab_handler = hab_handler;
    iter->node_handler = node_handler;
    iter->resource_handler = resource_handler;
    iter->datapoint_handler = datapoint_handler;

}

int bdm_list_traverse(bdm_list_iterator_t * iter) {

    GPtrArray * bdm_list = iter->bdm_list;
    int r;

    if(bdm_list == NULL) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "%s(): NULL BDM list", __FUNCTION__);
        return 0;
    }

    for (; iter->bi < bdm_list->len; iter->bi++) {
        GPtrArray * hab_list = NULL;
	bionet_bdm_t * bdm = g_ptr_array_index(bdm_list, iter->bi);
	if (NULL == bdm) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "%s(): Failed to get BDM %d from BDM list", 
                  __FUNCTION__, iter->bi);
	    continue;
	}

        if(iter->usr_data) {
            r = iter->bdm_handler(bdm, iter->usr_data);
            if(r) {
                return r;
            }
        }

        hab_list = bionet_bdm_get_hab_list(bdm);
        for (; iter->hi < hab_list->len; iter->hi++) {
            bionet_hab_t * hab = g_ptr_array_index(hab_list, iter->hi);
            if (NULL == hab) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
                      "%s(): Failed to get HAB %d from array of HABs", 
                      __FUNCTION__, iter->hi);
                continue;
            }

            if(iter->hab_handler) {
                r = iter->hab_handler(bdm, hab, iter->usr_data);
                if(r) {
                    return r;
                }
            }

            for (; iter->ni < bionet_hab_get_num_nodes(hab); iter->ni++) {
                bionet_node_t * node = bionet_hab_get_node_by_index(hab, iter->ni);
                if (NULL == node) {
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
                          "%s(): Failed to get Node %d hab %s", 
                          __FUNCTION__, iter->ni, bionet_hab_get_name(hab));
                    continue;
                }

                if(iter->node_handler) {
                    r = iter->node_handler(bdm, node, iter->usr_data);
                    if(r) {
                        return r;
                    }
                }

                // No point to traverse further if no handler
                if(iter->datapoint_handler || iter->resource_handler) {
                    for (; iter->ri < bionet_node_get_num_resources(node); iter->ri++) {
                        bionet_resource_t *resource = 
                            bionet_node_get_resource_by_index(node, iter->ri);
                        if (NULL == resource) {
                            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
                                  "%s(): Failed to get Resource %d from node %s", 
                                  __FUNCTION__, iter->ni, bionet_node_get_name(node));
                            continue;
                        }

                        if(iter->resource_handler) {
                            r = iter->resource_handler(bdm, resource, iter->usr_data);
                            if(r) {
                                return r;
                            }
                        }

                        if(iter->datapoint_handler) {
                            for (;
                                 iter->di < bionet_resource_get_num_datapoints(resource);
                                 iter->di++) 
                            {
                                bionet_datapoint_t * d = 
                                    bionet_resource_get_datapoint_by_index(resource, iter->di);

                                if (NULL == d) {
                                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
                                          "%s(): Failed to get datapoint %d from Resource %s",
                                          __FUNCTION__,
                                          iter->di, bionet_resource_get_name(resource));
                                }

                                r = iter->datapoint_handler(bdm, d, iter->usr_data);
                                if(r) {
                                    return r;
                                }

                            } // for-each datapoint
                            iter->di=0;
                        }
                    } // for-each resource
                    iter->ri=0;
                }
            } // for-each node
            iter->ni=0;
        } // for-each hab
        iter->hi = 0;
    } // for-each bdm
    iter->bi = 0;

    // Done
    return 0;
}

