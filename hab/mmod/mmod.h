
//
// Copyright (C) 2004-2008, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G and NNC06CB40C.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of GNU General Public License version 2, as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//

#ifndef MMOD_H
#define MMOD_H


#include "hardware-abstractor.h"


#define MMOD_HAB_TYPE          "MMOD"
#define DEFAULT_HEARTBEAT_TIME 2
#define DEFAULT_USB_DEV        "/dev/ttyUSB1"


/**
 * Callback for sending message (settings) to the gateway
 *
 * @param[in] node_id ID of the node to send it to
 * @param[in] resource_id ID of the resource for which the value is to be set
 * @param[in] value Value to set to node_id.resource_id
 */
void cb_set_resource(bionet_resource_t *resource,
		     const bionet_datapoint_value_t *value);


#endif /* MMOD_H */
