
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

#include <string.h>
#include <stdio.h>

#include "bionet-resource.h"
#include "bionet-node.h"
#include "bionet-hab.h"
#include "set-resource.h"
#include "serialsource.h"
#include "mmodsettingsmsg.h"
#include "message.h"
#include "mmod_message.h"

extern serial_source gw_src;
extern bionet_hab_t * this;

void cb_set_resource(const char *node_id, 
		     const char *resource_id, 
		     const char *value)
{
    uint8_t pkt[MMODSETTINGSMSG_SIZE + 8] = { 0 };
    uint16_t uiVal;
    bionet_node_t *node;
    bionet_resource_t *resource;
    tmsg_t new_settings;
    bionet_datapoint_t * dp;

    new_settings.data = malloc(MMODSETTINGSMSG_SIZE);
    new_settings.len = MMODSETTINGSMSG_SIZE;

    pkt[0] = 0;
    pkt[1] = 0xFF;
    pkt[2] = 0xFF;
    pkt[3] = 0;
    pkt[5] = MMODSETTINGSMSG_SIZE;
    pkt[7] = MMODSETTINGSMSG_AM_TYPE;

    /* get the node we are referring to */
    if (NULL == this)
    {
	return;
    }
    node = bionet_hab_get_node_by_id(this, node_id);
    if (NULL == node)
    {
	return;
    }

    /* set the correct node id */
    uiVal = strtoul(node_id, NULL, 10);
    MMODSETTINGSMSG_node_id_set(&new_settings, uiVal);

    /* update resource requested */
    if (0 == strncmp(resource_id, "SampleInterval", 
			  strlen("SampleInterval")))
    {
	uiVal = strtoul(value, NULL, 0);
    }
    else
    {
	resource = bionet_node_get_resource_by_id(node, "SampleInterval");
	if (NULL == resource)
	{
	    uiVal = 0;
	}
	else
	{
	    dp = bionet_resource_get_datapoint_by_index(resource, 0);
	    if (NULL == dp)
	    {
		uiVal = 0;
	    }
	    else
	    {
		uiVal = dp->value.uint16_v;
	    }
	}
    }
    MMODSETTINGSMSG_sample_interval_set(&new_settings, uiVal);
    
    if (0 == strncmp(resource_id, "NumAccelSamples", 
			  strlen("NumAccelSamples")))
    {
	uiVal = strtoul(value, NULL, 0);
    }
    else
    {
	resource = bionet_node_get_resource_by_id(node, "NumAccelSamples");
	if (NULL == resource)
	{
	    uiVal = 0;
	}
	else
	{
	    dp = bionet_resource_get_datapoint_by_index(resource, 0);
	    if (NULL == dp)
	    {
		uiVal = 0;
	    }
	    else
	    {
		uiVal = dp->value.uint16_v;
	    }
	}
    }
    MMODSETTINGSMSG_num_accel_samples_set(&new_settings, uiVal);
    
    if (0 == strncmp(resource_id, "AccelSampleInterval", 
			  strlen("AccelSampleInterval")))
    {
	uiVal = strtoul(value, NULL, 0);
    }
    else
    {
	resource = bionet_node_get_resource_by_id(node, "AccelSampleInterval");
	if (NULL == resource)
	{
	    uiVal = 0;
	}
	else
	{
	    dp = bionet_resource_get_datapoint_by_index(resource, 0);
	    if (NULL == dp)
	    {
		uiVal = 0;
	    }
	    else
	    {
		uiVal = dp->value.uint16_v;
	    }
	}
    }
    MMODSETTINGSMSG_accel_sample_interval_set(&new_settings, uiVal);
    
    if (0 == strncmp(resource_id, "HeartbeatTime", 
			  strlen("HeartbeatTime")))
    {
	uiVal = strtoul(value, NULL, 0);
    }
    else
    {
	resource = bionet_node_get_resource_by_id(node, "HeartbeatTime");
	if (NULL == resource)
	{
	    uiVal = 0;
	}
	else
	{
	    dp = bionet_resource_get_datapoint_by_index(resource, 0);
	    if (NULL == dp)
	    {
		uiVal = 0;
	    }
	    else
	    {
		uiVal = dp->value.uint16_v;
	    }
	}
    }
    MMODSETTINGSMSG_heartbeat_time_set(&new_settings, uiVal);

    if (0 == strncmp(resource_id, "AccelAxis", 
			  strlen("AccelAxis")))
    {
	switch (value[0])
	{
	case 'x':
	case 'X':
	    uiVal = ACCEL_FLAG_X;
	    break;
	case 'y':
	case 'Y':
	    uiVal = ACCEL_FLAG_Y;
	    break;
	default:
	    uiVal = ACCEL_FLAG_X | ACCEL_FLAG_Y;
	    break;
	}
    }
    else
    {
	resource = bionet_node_get_resource_by_id(node, "AccelAxis");
	if (NULL == resource)
	{
	    uiVal = ACCEL_FLAG_X;
	}
	else
	{
	    dp = bionet_resource_get_datapoint_by_index(resource, 0);
	    if (NULL == dp)
	    {
		uiVal = 0;
	    }
	    else
	    {
		uiVal = dp->value.uint16_v;
	    }

	    switch (dp->value.string_v[0])
	    {
	    case 'X':
		uiVal = ACCEL_FLAG_X;
		break;
	    case 'Y':
		uiVal = ACCEL_FLAG_Y;
		break;
	    default:
		uiVal = ACCEL_FLAG_X | ACCEL_FLAG_Y;
		break;
	    }
  	}
    }
    MMODSETTINGSMSG_accel_flags_set(&new_settings, uiVal);

    memcpy(&pkt[8], new_settings.data, new_settings.len);
    
    write_serial_packet(gw_src, pkt, MMODSETTINGSMSG_SIZE + 8);

    free(new_settings.data);

    return;
} /* cb_set_resource() */
