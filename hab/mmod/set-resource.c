
//
// Copyright (C) 2008, Regents of the University of Colorado.
//

#include <string.h>
#include <stdio.h>

#include "bionet-resource.h"
#include "bionet-node.h"
#include "bionet-hab.h"
#include "mmod.h"
#include "serialsource.h"
#include "mmodsettingsmsg.h"
#include "message.h"
#include "mmod_message.h"
#include "set-resource.h"

extern serial_source gw_src;
extern bionet_hab_t * mmod_hab;

void cb_set_resource(bionet_resource_t *resource,
		     const bionet_datapoint_value_t *value)
{
    uint8_t pkt[MMODSETTINGSMSG_SIZE + 8] = { 0 };
    uint16_t uiVal;
    const bionet_node_t *node;
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
    if (NULL == mmod_hab)
    {
	return;
    }
    node = resource->node;
    if (NULL == node)
    {
	return;
    }

    /* set the correct node id */
    uiVal = strtoul(node->id, NULL, 10);
    MMODSETTINGSMSG_node_id_set(&new_settings, uiVal);

    /* update resource requested */
    if (0 == strncmp(resource->id, "SampleInterval", 
			  strlen("SampleInterval")))
    {
	uiVal = value->uint16_v;
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
    
    if (0 == strncmp(resource->id, "NumAccelSamples", 
			  strlen("NumAccelSamples")))
    {
	uiVal = value->uint16_v;
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
    
    if (0 == strncmp(resource->id, "AccelSampleInterval", 
			  strlen("AccelSampleInterval")))
    {
	uiVal = value->uint16_v;
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
    
    if (0 == strncmp(resource->id, "HeartbeatTime", 
			  strlen("HeartbeatTime")))
    {
	uiVal = value->uint16_v;
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

    if (0 == strncmp(resource->id, "AccelAxis", 
			  strlen("AccelAxis")))
    {
	switch (value->string_v[0])
	{
	case 'n':
	case 'N':
	    uiVal = 0;
	    break;
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
	    case 'N':
		uiVal = 0;
		break;
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
