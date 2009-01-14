
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
#include "set-resource.h"

static struct timeval timeval_table[256];
uint16_t current_tv_index = 0;

extern bionet_hab_t * mmod_hab;

static uint16_t accel_thres = 0;

extern uint16_t heartbeat_time;
static uint16_t flags = ACCEL_FLAG_X;

static uint16_t last = 0;

int msg_gen_process(uint8_t *msg, ssize_t len)
{
    tmsg_t t = { msg, (size_t)len };
    bionet_node_t *node;
    char node_id[8];
    struct timeval tv;
    bionet_resource_t * resource;
    struct timeval tv_accel;
    uint16_t offset;
    uint16_t tv_index;

    snprintf(&node_id[0], 8, "%04u", MMODGENMSG_node_id_get(&t));
    node = bionet_hab_get_node_by_id(mmod_hab, &node_id[0]);

    if (0 > gettimeofday(&tv, NULL))
    {
	g_warning("error with gettimeofday: %s", strerror(errno));
    }

    /* if this node doesn't exist yet, create it */
    if (NULL == node)
    {
	return 0;
    }

    resource = bionet_node_get_resource_by_id(node, "Timestamp");
    if (NULL == resource)
    {
	fprintf(stderr, "Failed to get resource: Timestamp\n");
    }
    else
    {
	uint32_t secs;
	tv_index = MMODGENMSG_timestamp_id_get(&t);
	tv_accel = timeval_table[tv_index];
	offset = MMODGENMSG_offset_get(&t);

	secs = offset / 1000;
	tv_accel.tv_sec += secs;
	tv_accel.tv_usec += ((uint32_t)offset - (secs * 1000)) * 1000;
	//handle rollover
	if (tv_accel.tv_usec < timeval_table[tv_index].tv_usec)
	{
	    tv_accel.tv_sec++;
	}
	if (tv_accel.tv_usec >= 1000000)
	{
	    tv_accel.tv_usec -= 1000000;
	    tv_accel.tv_sec++;
	}
    }

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
				  &tv_accel);
    }
    
    resource = bionet_node_get_resource_by_id(node, "RawAccel-X");
    if (NULL == resource)
    {
	fprintf(stderr, "Failed to get resource: RawAccel-X\n");
    }
    else
    {
	bionet_resource_set_uint16(resource, MMODGENMSG_accel_x_get(&t), &tv_accel);
    }

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
				  &tv_accel);
    }
    
    resource = bionet_node_get_resource_by_id(node, "RawAccel-Y");
    if (NULL == resource)
    {
	fprintf(stderr, "Failed to get resource: RawAccel-Y\n");
    }
    else
    {
	bionet_resource_set_uint16(resource, MMODGENMSG_accel_y_get(&t), &tv_accel);
    }

    
#if DEBUG						
    fprintf(stderr, "Reporting general update...\n");
#endif						
    hab_report_datapoints(node);

    return 0;
} /* msg_gen_process() */


int msg_settings_process(uint8_t *msg, ssize_t len)
{
    char node_id[8];
    tmsg_t t = { msg, (size_t)len };
    bionet_node_t *node;
    bionet_resource_t *resource;
    struct timeval tv;

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

	/* create Acceleration Threshold resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT16,
				       BIONET_RESOURCE_FLAVOR_PARAMETER, 
				       "AccelThres");
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
	    if (bionet_resource_set_uint16(resource, MMODSETTINGSMSG_thres_accel_get(&t), &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	    accel_thres = MMODSETTINGSMSG_thres_accel_get(&t);
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

	resource = bionet_resource_new(node,
				       BIONET_RESOURCE_DATA_TYPE_UINT16,
				       BIONET_RESOURCE_FLAVOR_PARAMETER,
				       "Timestamp");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: Timestamp\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: Timestamp\n");
	    }
	    bionet_resource_set_uint16(resource, 0, &tv);
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

	resource = bionet_node_get_resource_by_id(node, "Timestamp");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get resource: Timestamp\n");
	}
	else
	{
	    bionet_resource_set_uint16(resource, MMODSETTINGSMSG_timestamp_id_get(&t), &tv);
	}
    }

    hab_report_datapoints(node);

    resource = bionet_node_get_resource_by_id(node, "Timestamp");
    current_tv_index++;
    if (0 > gettimeofday(&timeval_table[current_tv_index & 0xFF], NULL))
    {
	g_warning("error with gettimeofday: %s", strerror(errno));
    }
    else
    {
	bionet_value_t * tsv = bionet_value_new_uint16(resource, 
						       current_tv_index);
	cb_set_resource(resource, tsv);
	bionet_value_free(tsv);
    }

    return 0;
} /* msg_settings_process() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
