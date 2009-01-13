
//
// Copyright (C) 2008, Regents of the University of Colorado.
//

#include <string.h>
#include <stdio.h>

#include "bionet-util.h"
#include "mmod.h"
#include "serialsource.h"
#include "mmodsettingsmsg.h"
#include "message.h"
#include "mmod_message.h"
#include "set-resource.h"

extern serial_source gw_src;
extern bionet_hab_t * mmod_hab;
uint16_t current_tv_index;

void cb_set_resource(bionet_resource_t *resource,
		     bionet_value_t *value)
{
    uint8_t pkt[MMODSETTINGSMSG_SIZE + 8] = { 0 };
    uint16_t uiVal;
    const bionet_node_t *node;
    tmsg_t new_settings;
    bionet_datapoint_t * dp;
    bionet_value_t *val;
    bionet_resource_t *lr;

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
    node = bionet_resource_get_node(resource);
    if (NULL == node)
    {
	return;
    }

    /* set the correct node id */
    uiVal = strtoul(bionet_node_get_id(node), NULL, 10);
    MMODSETTINGSMSG_node_id_set(&new_settings, uiVal);

    /* update resource requested */
    if (0 == strncmp(bionet_resource_get_id(resource), "SampleInterval", 
		     strlen("SampleInterval")))
    {
	bionet_value_get_uint16(value, &uiVal);
    }
    else
    {
	lr = bionet_node_get_resource_by_id(node, "SampleInterval");
	if (NULL == lr)
	{
	    uiVal = 0;
	}
	else
	{
	    dp = bionet_resource_get_datapoint_by_index(lr, 0);
	    if (NULL == dp)
	    {
		uiVal = 0;
	    }
	    else
	    {
		val = bionet_datapoint_get_value(dp);
		bionet_value_get_uint16(val, &uiVal);
	    }
	}
    }
    MMODSETTINGSMSG_sample_interval_set(&new_settings, uiVal);
    
    if (0 == strncmp(bionet_resource_get_id(resource), "NumAccelSamples", 
			  strlen("NumAccelSamples")))
    {
	bionet_value_get_uint16(value, &uiVal);
    }
    else
    {
	lr = bionet_node_get_resource_by_id(node, "NumAccelSamples");
	if (NULL == lr)
	{
	    uiVal = 0;
	}
	else
	{
	    dp = bionet_resource_get_datapoint_by_index(lr, 0);
	    if (NULL == dp)
	    {
		uiVal = 0;
	    }
	    else
	    {
		val = bionet_datapoint_get_value(dp);
		bionet_value_get_uint16(val, &uiVal);
	    }
	}
    }
    MMODSETTINGSMSG_num_accel_samples_set(&new_settings, uiVal);
    
    if (0 == strncmp(bionet_resource_get_id(resource), "AccelSampleInterval", 
			  strlen("AccelSampleInterval")))
    {
	bionet_value_get_uint16(value, &uiVal);
    }
    else
    {
	lr = bionet_node_get_resource_by_id(node, "AccelSampleInterval");
	if (NULL == lr)
	{
	    uiVal = 0;
	}
	else
	{
	    dp = bionet_resource_get_datapoint_by_index(lr, 0);
	    if (NULL == dp)
	    {
		uiVal = 0;
	    }
	    else
	    {
		val = bionet_datapoint_get_value(dp);
		bionet_value_get_uint16(val, &uiVal);
	    }
	}
    }
    MMODSETTINGSMSG_accel_sample_interval_set(&new_settings, uiVal);
    
    if (0 == strncmp(bionet_resource_get_id(resource), "HeartbeatTime", 
			  strlen("HeartbeatTime")))
    {
	if (bionet_value_get_uint16(value, &uiVal))
	{
	    fprintf(stderr, "Failed to get value for HeartbeatTime\n");
	}
    }
    else
    {
	lr = bionet_node_get_resource_by_id(node, "HeartbeatTime");
	if (NULL == lr)
	{
	    uiVal = 0;
	}
	else
	{
	    dp = bionet_resource_get_datapoint_by_index(lr, 0);
	    if (NULL == dp)
	    {
		uiVal = 0;
	    }
	    else
	    {
		val = bionet_datapoint_get_value(dp);
		bionet_value_get_uint16(val, &uiVal);
	    }
	}
    }
    MMODSETTINGSMSG_heartbeat_time_set(&new_settings, uiVal);

    if (0 == strncmp(bionet_resource_get_id(resource), "AccelAxis", 
			  strlen("AccelAxis")))
    {
	char * tmp_str;
	bionet_value_get_str(value, &tmp_str);
	switch (tmp_str[0])
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
	lr = bionet_node_get_resource_by_id(node, "AccelAxis");
	if (NULL == lr)
	{
	    uiVal = ACCEL_FLAG_X;
	}
	else
	{
	    dp = bionet_resource_get_datapoint_by_index(lr, 0);
	    if (NULL == dp)
	    {
		uiVal = 0;
	    }
	    else
	    {
		char * tmp_str;
		val = bionet_datapoint_get_value(dp);
		bionet_value_get_str(val, &tmp_str);
		switch (tmp_str[0])
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
    }
    MMODSETTINGSMSG_accel_flags_set(&new_settings, uiVal);

    if (0 == strncmp(bionet_resource_get_id(resource), "Timestamp", 
			  strlen("Timestamp")))
    {
	if (bionet_value_get_uint16(value, &uiVal))
	{
	    fprintf(stderr, "Failed to get value for Timestamp\n");
	}
	MMODSETTINGSMSG_is_ts_update_set(&new_settings, 1);
    }
    else
    {
	lr = bionet_node_get_resource_by_id(node, "Timestamp");
	if (NULL == lr)
	{
	    fprintf(stderr, "no timestamp resource");
	    uiVal = 0;
	}
	else
	{
	    dp = bionet_resource_get_datapoint_by_index(lr, 0);
	    if (NULL == dp)
	    {
		uiVal = current_tv_index;
	    }
	    else
	    {
		val = bionet_datapoint_get_value(dp);
		bionet_value_get_uint16(val, &uiVal);
	    }
	}
	MMODSETTINGSMSG_is_ts_update_set(&new_settings, 0);
    }
    MMODSETTINGSMSG_timestamp_id_set(&new_settings, uiVal);

    memcpy(&pkt[8], new_settings.data, new_settings.len);
    
    write_serial_packet(gw_src, pkt, MMODSETTINGSMSG_SIZE + 8);

    //send timestamp out to all nodes
    bionet_hab_t * hab = bionet_resource_get_hab(resource);
    bionet_node_t * this_node = bionet_resource_get_node(resource);
    bionet_node_t * first_node;
    first_node = bionet_hab_get_node_by_index(hab, 0);
    if (first_node == this_node)
    {
	MMODSETTINGSMSG_node_id_set(&new_settings, 0);
	MMODSETTINGSMSG_is_ts_update_set(&new_settings, 1);
	memcpy(&pkt[8], new_settings.data, new_settings.len);
	write_serial_packet(gw_src, pkt, MMODSETTINGSMSG_SIZE + 8);
    }

    free(new_settings.data);

    return;
} /* cb_set_resource() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
