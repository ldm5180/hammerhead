
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef SET_RESOURCE_H
#define SET_RESOURCE_H

#include "hardware-abstractor.h"

/**
 * Callback for sending message (settings) to the gateway
 *
 * @param[in] node_id ID of the node to send it to
 * @param[in] resource_id ID of the resource for which the value is to be set
 * @param[in] value Value to set to node_id.resource_id
 */
void cb_set_resource(bionet_resource_t *resource,
		     bionet_value_t *value);

#endif
