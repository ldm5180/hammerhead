
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

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "mmod.h"
#include "hardware-abstractor.h"
#include "mmod_message.h"
#include "mmodgenmsg.h"
#include "mmodsettingsmsg.h"
#include "message.h"
#include "bionet-hab.h"
#include "mts310_cook.h"
#include "serialsource.h"
#include "message.h"

extern bionet_hab_t * mmod_hab;


extern uint16_t heartbeat_time;
static uint16_t flags = ACCEL_FLAG_X;

static uint16_t last = 0;
static uint16_t missed = 0;

int msg_gen_process(uint8_t *msg, ssize_t len)
{
    tmsg_t t = { msg, (size_t)len };
    bionet_node_t *node;
    char node_id[8];
    struct timeval tv;
    bionet_resource_t * resource;

    snprintf(&node_id[0], 8, "%04u", MMODGENMSG_node_id_get(&t));
    node = bionet_hab_get_node_by_id(mmod_hab, &node_id[0]);

    if (0 > gettimeofday(&tv, NULL))
    {
	g_warning("error with gettimeofday: %s", strerror(errno));
    }

    /* if this node doesn't exist yet, create it */
    if (NULL == node)
    {

	last = MMODGENMSG_accel_y_get(&t);
	node = bionet_node_new(mmod_hab, &node_id[0]);
	if (NULL == node)
	{
	    fprintf(stderr, "Failed to create new node\n");
	    return 1;
	}
	bionet_hab_add_node(mmod_hab, node);


	/* create accel-x resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_FLOAT,
				       BIONET_RESOURCE_FLAVOR_SENSOR, 
				       "Accel-X");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: Accel-X\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: Accel-X\n");
	    }
	    if (bionet_resource_set_float(resource, 0, &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	}

	/* create raw accel-x resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT16,
				       BIONET_RESOURCE_FLAVOR_SENSOR, 
				       "RawAccel-X");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: RawAccel-X\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: RawAccel-X\n");
	    }
	    if (bionet_resource_set_uint16(resource, 0, &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	}

	/* create accel-y resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_FLOAT,
				       BIONET_RESOURCE_FLAVOR_SENSOR, 
				       "Accel-Y");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: Accel-Y\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: Accel-Y\n");
	    }
	    if (bionet_resource_set_float(resource, 0, &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	}

	/* create raw accel-y resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT16,
				       BIONET_RESOURCE_FLAVOR_SENSOR, 
				       "RawAccel-Y");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: RawAccel-Y\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: RawAccel-Y\n");
	    }
	    if (bionet_resource_set_uint16(resource, 0, &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	}

	/* create Sample Interval resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT16,
				       BIONET_RESOURCE_FLAVOR_PARAMETER, 
				       "SampleInterval");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: SampleInterval\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: SampleInterval\n");
	    }
	    if (bionet_resource_set_uint16(resource, 0, &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	}

	/* create Num of Accel Samples resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT16,
				       BIONET_RESOURCE_FLAVOR_PARAMETER, 
				       "NumAccelSamples");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: NumAccelSamples\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: NumAccelSamples\n");
	    }
	    if (bionet_resource_set_uint16(resource, 0, &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	}

	/* create Accel Sample Interval resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT16,
				       BIONET_RESOURCE_FLAVOR_PARAMETER, 
				       "AccelSampleInterval");
	if (NULL == resource)
	{
	    fprintf(stderr, 
		    "Failed to get new resource: AccelSampleInterval\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, 
			"Failed to add resource: AccelSampleInterval\n");
	    }
	    if (bionet_resource_set_uint16(resource, 0, &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	}

	/* create Heartbeat Time resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT16,
				       BIONET_RESOURCE_FLAVOR_PARAMETER, 
				       "HeartbeatTime");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: HeartbeatTime\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: HeartbeatTime\n");
	    }
	    if (bionet_resource_set_uint16(resource, 0, &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	}

	/* create  Accel Axis resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_STRING,
				       BIONET_RESOURCE_FLAVOR_PARAMETER, 
				       "AccelAxis");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: AccelAxis\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: AccelAxis\n");
	    }
	    if (bionet_resource_set_str(resource, strdup("X-Axis"), &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	}

	/* create counter resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT16,
				       BIONET_RESOURCE_FLAVOR_SENSOR, 
				       "MsgCounter");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: MsgCounter\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: MsgCounter\n");
	    }
	    if (bionet_resource_set_uint16(resource, last, &tv))
	    {
		fprintf(stderr, "Failed to set resource: MsgCounter\n"); 
	    }
	}

	/* create counter resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT16,
				       BIONET_RESOURCE_FLAVOR_SENSOR, 
				       "MissedMsgs");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: MissedMsgs\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: MissedMsgs\n");
	    }
	    if (bionet_resource_set_uint16(resource, 0, &tv))
	    {
		fprintf(stderr, "Failed to set resource: MissedMsgs\n"); 
	    }
	}

	/* add node for real */
	if (hab_report_new_node(node))
	{
	    g_error("Error reporting new node %s to hab", &node_id[0]);
	}
    }
    else
    {
	if (ACCEL_FLAG_X & flags)
	{
	    if (MMODGENMSG_accel_x_get(&t) >= 20)
	    {
		struct timeval tmp_tv = tv;
		bionet_value_t * value;
		float content;
		tmp_tv.tv_sec--;
		resource = bionet_node_get_resource_by_id(node, "Accel-X");
		value = bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0));
		bionet_value_get_float(value, &content);
		bionet_resource_set_float(resource, 
					  mts310_cook_accel(MMODGENMSG_node_id_get(&t), 
							    X_AXIS,
							    content),
					  &tmp_tv);
	    }
	 
	    if (MMODGENMSG_accel_x_get(&t) >= 20)
	    {
		struct timeval tmp_tv = tv;
		bionet_value_t * value;
		uint16_t content;
		tmp_tv.tv_sec--;
		resource = bionet_node_get_resource_by_id(node, "RawAccel-X");
		value = bionet_datapoint_get_value(bionet_resource_get_datapoint_by_index(resource, 0));
		bionet_value_get_uint16(value, &content);
		bionet_resource_set_uint16(resource, content, &tmp_tv);
	    }

	    hab_report_datapoints(node);
	}

	if (0 == (ACCEL_FLAG_X & flags))
	{
	    /* using accel-x as a counter */
	    resource = bionet_node_get_resource_by_id(node, "MsgCounter");
	    if (NULL == resource)
	    {
		fprintf(stderr, "Failed to get resource: MsgCounter\n");
	    }
	    else
	    {
		/* set the latest msg count */
		bionet_resource_set_uint16(resource, MMODGENMSG_accel_x_get(&t), &tv);

		if ((last != 0) && (MMODGENMSG_accel_x_get(&t) > 1) 
		    && (last + 1 != MMODGENMSG_accel_x_get(&t)))
		{
		    missed = last - MMODGENMSG_accel_x_get(&t);
		}
		last = MMODGENMSG_accel_x_get(&t);
	    }
	}
	else
	{
	    resource = bionet_node_get_resource_by_id(node, "Accel-X");
	    if (NULL == resource)
	    {
		fprintf(stderr, "Failed to get resource: Accel-X\n");
	    }
	    else
	    {
		bionet_resource_set_float(resource, 
					  mts310_cook_accel(MMODGENMSG_node_id_get(&t), 
							    X_AXIS,
							    MMODGENMSG_accel_x_get(&t)),
					  &tv);
	    }
	
	    resource = bionet_node_get_resource_by_id(node, "RawAccel-X");
	    if (NULL == resource)
	    {
		fprintf(stderr, "Failed to get resource: RawAccel-X\n");
	    }
	    else
	    {
		bionet_resource_set_uint16(resource, MMODGENMSG_accel_x_get(&t), &tv);
	    }
	}

	if ((ACCEL_FLAG_X & flags) && (0 == (ACCEL_FLAG_Y & flags)))
	{
	    /* using accel-y as a counter */
	    resource = bionet_node_get_resource_by_id(node, "MsgCounter");
	    if (NULL == resource)
	    {
		fprintf(stderr, "Failed to get resource: MsgCounter\n");
	    }
	    else
	    {
		/* set the latest msg count */
		bionet_resource_set_uint16(resource, MMODGENMSG_accel_y_get(&t), &tv);

		if ((last != 0) && (MMODGENMSG_accel_y_get(&t) > 1) 
		    && (last + 1 != MMODGENMSG_accel_y_get(&t)))
		{
		    missed = last - MMODGENMSG_accel_y_get(&t);
		}
		last = MMODGENMSG_accel_y_get(&t);
	    }
	}
	else
	{
	    resource = bionet_node_get_resource_by_id(node, "Accel-Y");
	    if (NULL == resource)
	    {
		fprintf(stderr, "Failed to get resource: Accel-Y\n");
	    }
	    else
	    {
		bionet_resource_set_float(resource,
					  mts310_cook_accel(MMODGENMSG_node_id_get(&t), 
							    Y_AXIS,
							    MMODGENMSG_accel_y_get(&t)),
					  &tv);
	    }
	    
	    resource = bionet_node_get_resource_by_id(node, "RawAccel-Y");
	    if (NULL == resource)
	    {
		fprintf(stderr, "Failed to get resource: RawAccel-Y\n");
	    }
	    else
	    {
		bionet_resource_set_uint16(resource, MMODGENMSG_accel_y_get(&t), &tv);
	    }
	}

	resource = bionet_node_get_resource_by_id(node,
						  "MissedMsgs");
	if (NULL != resource)
	{
	    bionet_resource_set_uint16(resource, missed, &tv);
	}

#if DEBUG
	fprintf(stderr, "Reporting general update...\n");
#endif
	hab_report_datapoints(node);
    }

    return 0;
} /* msg_gen_process() */


int msg_settings_process(uint8_t *msg, ssize_t len)
{
    char node_id[8];
    tmsg_t t = { msg, (size_t)len };
    bionet_node_t *node;
    bionet_resource_t *resource;
    struct timeval tv;

    fprintf(stderr, "incoming settings msg\n");

    if (NULL == mmod_hab)
    {
	return 0;
    }

    snprintf(&node_id[0], 8, "%04u", MMODSETTINGSMSG_node_id_get(&t));
    node = bionet_hab_get_node_by_id(mmod_hab, &node_id[0]);

    if (0 > gettimeofday(&tv, NULL))
    {
	g_warning("error with gettimeofday: %s", strerror(errno));
    }

    /* if this node doesn't exist yet, forget it */
    if (NULL == node)
    {
	last = MMODGENMSG_accel_y_get(&t);
	node = bionet_node_new(mmod_hab, &node_id[0]);
	if (NULL == node)
	{
	    fprintf(stderr, "Failed to create new node\n");
	    return 1;
	}
	bionet_hab_add_node(mmod_hab, node);


	/* create accel-x resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_FLOAT,
				       BIONET_RESOURCE_FLAVOR_SENSOR, 
				       "Accel-X");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: Accel-X\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: Accel-X\n");
	    }
	    if (bionet_resource_set_float(resource, 0, &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	}

	/* create raw accel-x resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT16,
				       BIONET_RESOURCE_FLAVOR_SENSOR, 
				       "RawAccel-X");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: RawAccel-X\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: RawAccel-X\n");
	    }
	    if (bionet_resource_set_uint16(resource, 0, &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	}

	/* create accel-y resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_FLOAT,
				       BIONET_RESOURCE_FLAVOR_SENSOR, 
				       "Accel-Y");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: Accel-Y\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: Accel-Y\n");
	    }
	    if (bionet_resource_set_float(resource, 0, &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	}

	/* create raw accel-y resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT16,
				       BIONET_RESOURCE_FLAVOR_SENSOR, 
				       "RawAccel-Y");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: RawAccel-Y\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: RawAccel-Y\n");
	    }
	    if (bionet_resource_set_uint16(resource, 0, &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	}

	/* create Sample Interval resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT16,
				       BIONET_RESOURCE_FLAVOR_PARAMETER, 
				       "SampleInterval");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: SampleInterval\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: SampleInterval\n");
	    }
	    if (bionet_resource_set_uint16(resource, MMODSETTINGSMSG_sample_interval_get(&t), &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	}

	/* create Num of Accel Samples resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT16,
				       BIONET_RESOURCE_FLAVOR_PARAMETER, 
				       "NumAccelSamples");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: NumAccelSamples\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: NumAccelSamples\n");
	    }
	    if (bionet_resource_set_uint16(resource, MMODSETTINGSMSG_num_accel_samples_get(&t), &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	}

	/* create Accel Sample Interval resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT16,
				       BIONET_RESOURCE_FLAVOR_PARAMETER, 
				       "AccelSampleInterval");
	if (NULL == resource)
	{
	    fprintf(stderr, 
		    "Failed to get new resource: AccelSampleInterval\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, 
			"Failed to add resource: AccelSampleInterval\n");
	    }
	    if (bionet_resource_set_uint16(resource, MMODSETTINGSMSG_accel_sample_interval_get(&t), &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	}

	/* create Heartbeat Time resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT16,
				       BIONET_RESOURCE_FLAVOR_PARAMETER, 
				       "HeartbeatTime");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: HeartbeatTime\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: HeartbeatTime\n");
	    }
	    if (bionet_resource_set_uint16(resource, MMODSETTINGSMSG_heartbeat_time_get(&t), &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	    if (MMODSETTINGSMSG_heartbeat_time_get(&t) < heartbeat_time)
	    {
		heartbeat_time = (uint32_t)MMODSETTINGSMSG_heartbeat_time_get(&t);
	    }
	}

	/* create  Accel Axis resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_STRING,
				       BIONET_RESOURCE_FLAVOR_PARAMETER, 
				       "AccelAxis");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: AccelAxis\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: AccelAxis\n");
	    }
	    flags = MMODSETTINGSMSG_accel_flags_get(&t);
	    if (flags & ACCEL_FLAG_X)
	    {
		if (flags & ACCEL_FLAG_Y)
		{
		    bionet_resource_set_str(resource, strdup("both"), &tv);
		}
		else
		{
		    bionet_resource_set_str(resource, strdup("X-Axis"), &tv);
		}
	    }
	    else if (flags & ACCEL_FLAG_Y)
	    {
		bionet_resource_set_str(resource, strdup("Y-Axis"), &tv);
	    }
	    else
	    {
		bionet_resource_set_str(resource, strdup("None"), &tv);
	    }
	}

	/* create counter resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT16,
				       BIONET_RESOURCE_FLAVOR_SENSOR, 
				       "MsgCounter");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: MsgCounter\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: MsgCounter\n");
	    }
	    if (bionet_resource_set_uint16(resource, last, &tv))
	    {
		fprintf(stderr, "Failed to set resource: MsgCounter\n"); 
	    }
	}

	/* create counter resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT16,
				       BIONET_RESOURCE_FLAVOR_SENSOR, 
				       "MissedMsgs");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: MissedMsgs\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: MissedMsgs\n");
	    }
	    if (bionet_resource_set_uint16(resource, 0, &tv))
	    {
		fprintf(stderr, "Failed to set resource: MissedMsgs\n"); 
	    }
	}

	/* add node for real */
	if (hab_report_new_node(node))
	{
	    g_error("Error reporting new node %s to hab", &node_id[0]);
	}
    }
    else
    {
	/* update the resources */
	resource = bionet_node_get_resource_by_id(node, "SampleInterval");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get resource: SampleInterval\n");
	}
	else
	{
	    bionet_resource_set_uint16(resource, MMODSETTINGSMSG_sample_interval_get(&t), &tv);
	}

	resource = bionet_node_get_resource_by_id(node, "NumAccelSamples");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get resource: NumAccelSamples\n");
	}
	else
	{
	    bionet_resource_set_uint16(resource, MMODSETTINGSMSG_num_accel_samples_get(&t), &tv);
	}

	resource = bionet_node_get_resource_by_id(node, "AccelSampleInterval");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get resource: AccelSampleInterval\n");
	}
	else
	{
	    bionet_resource_set_uint16(resource, MMODSETTINGSMSG_accel_sample_interval_get(&t), &tv);
	}

	resource = bionet_node_get_resource_by_id(node, "HeartbeatTime");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get resource: HeartbeatTime\n");
	}
	else
	{
	    bionet_resource_set_uint16(resource, MMODSETTINGSMSG_heartbeat_time_get(&t), &tv);
	}

	if (MMODSETTINGSMSG_heartbeat_time_get(&t) < heartbeat_time)
	{
	    heartbeat_time = (uint32_t)MMODSETTINGSMSG_heartbeat_time_get(&t);
	}

	resource = bionet_node_get_resource_by_id(node, "AccelAxis");
	flags = MMODSETTINGSMSG_accel_flags_get(&t);
	if (flags & ACCEL_FLAG_X)
	{
	    if (flags & ACCEL_FLAG_Y)
	    {
		bionet_resource_set_str(resource, strdup("both"), &tv);
	    }
	    else
	    {
		bionet_resource_set_str(resource, strdup("X-Axis"), &tv);
	    }
	}
	else if (flags & ACCEL_FLAG_Y)
	{
	    bionet_resource_set_str(resource, strdup("Y-Axis"), &tv);
	}
	else
	{
	    bionet_resource_set_str(resource, strdup("None"), &tv);
	}
    }

    hab_report_datapoints(node);
    return 0;
} /* msg_settings_process() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
