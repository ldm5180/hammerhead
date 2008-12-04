
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
#include "mmodaccelmsg.h"
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
    uint32_t mv;
    bionet_resource_t * resource;
    bionet_datapoint_value_t value;

#if DEBUG
    printf("    Message node: %04u\n", MMODGENMSG_node_id_get(&t)); 
    printf("    Voltage:      %04u\n", MMODGENMSG_volt_get(&t));
    printf("    Temperature:  %04u\n", MMODGENMSG_temp_get(&t));
    printf("    Photo:        %04u\n", MMODGENMSG_photo_get(&t));
    printf("    Accel X:      %04u\n", MMODGENMSG_accel_x_get(&t));
    printf("    Accel Y:      %04u\n", MMODGENMSG_accel_y_get(&t));
#endif /* DEBUG */

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
#if DEBUG
	fprintf(stderr, "Creating new node %s\n", &node_id[0]);
#endif
	node = bionet_node_new(mmod_hab, &node_id[0]);
	if (NULL == node)
	{
	    fprintf(stderr, "Failed to create new node\n");
	    return 1;
	}
	bionet_hab_add_node(mmod_hab, node);

	/* create voltage resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT32,
				       BIONET_RESOURCE_FLAVOR_SENSOR, 
				       "Voltage");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: Voltage\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: Voltage\n");
	    }
	    mv = value.uint32_v = mts310_cook_voltage(MMODGENMSG_volt_get(&t));
	    if (bionet_resource_set(resource, &value, &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	}
        /* create raw voltage resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT16,
				       BIONET_RESOURCE_FLAVOR_SENSOR, 
				       "RawVoltage");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: RawVoltage\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: RawVoltage\n");
	    }
	    value.uint16_v = MMODGENMSG_volt_get(&t);
	    if (bionet_resource_set(resource, &value, &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	}

	/* create temperature resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_FLOAT,
				       BIONET_RESOURCE_FLAVOR_SENSOR, 
				       "Temperature");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: Temperature\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: Temperature\n");
	    }
	    value.float_v = mts310_cook_temperature(MMODGENMSG_temp_get(&t));
	    if (bionet_resource_set(resource, &value, &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	}


	/* create raw temperature resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT16,
				       BIONET_RESOURCE_FLAVOR_SENSOR, 
				       "RawTemperature");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: RawTemperature\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: RawTemperature\n");
	    }
	    value.uint16_v = MMODGENMSG_temp_get(&t);
	    if (bionet_resource_set(resource, &value, &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	}

	/* create photo resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT16,
				       BIONET_RESOURCE_FLAVOR_SENSOR, 
				       "Photo");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: Photo\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: Photo\n");
	    }
	    value.uint16_v = mts310_cook_light(mv, MMODGENMSG_photo_get(&t));
	    if (bionet_resource_set(resource, &value, &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	}

	/* create raw photo resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT16,
				       BIONET_RESOURCE_FLAVOR_SENSOR, 
				       "RawPhoto");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: RawPhoto\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: RawPhoto\n");
	    }
	    value.uint16_v = MMODGENMSG_photo_get(&t);
	    if (bionet_resource_set(resource, &value, &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	}

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
	    value.float_v = 0;
	    if (bionet_resource_set(resource, &value, &tv))
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
	    value.uint16_v = 0;
	    if (bionet_resource_set(resource, &value, &tv))
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
	    value.float_v = 0;
	    if (bionet_resource_set(resource, &value, &tv))
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
	    value.uint16_v = 0;
	    if (bionet_resource_set(resource, &value, &tv))
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
	    value.uint16_v = 0;
	    if (bionet_resource_set(resource, &value, &tv))
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
	    value.uint16_v = 0;
	    if (bionet_resource_set(resource, &value, &tv))
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
	    value.uint16_v = 0;
	    if (bionet_resource_set(resource, &value, &tv))
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
	    value.uint16_v = 0;
	    if (bionet_resource_set(resource, &value, &tv))
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
	    if (bionet_resource_set_with_valuestr(resource, "X-Axis", &tv))
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
	    value.uint16_v = last;
	    if (bionet_resource_set(resource, &value, &tv))
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
	    value.uint16_v = 0;
	    if (bionet_resource_set(resource, &value, &tv))
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
	/* the node already exists, so just update the resource values */
	resource = bionet_node_get_resource_by_id(node, "Voltage");
	mv = mts310_cook_voltage(MMODGENMSG_volt_get(&t));
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get resource: Voltage\n");
	}
	else
	{
	    value.uint32_v = mv;
	    bionet_resource_set(resource, &value, &tv);
	}

	resource = bionet_node_get_resource_by_id(node, "RawVoltage");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get resource: RawVoltage\n");
	}
	else
	{
	    value.uint16_v = MMODGENMSG_volt_get(&t);
	    bionet_resource_set(resource, &value, &tv);
	}

	resource = bionet_node_get_resource_by_id(node, "Temperature");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get resource: Temperature\n");
	}
	else
	{
	    value.float_v = mts310_cook_temperature(MMODGENMSG_temp_get(&t));
	    bionet_resource_set(resource, &value, &tv);
	}

	resource = bionet_node_get_resource_by_id(node, "RawTemperature");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get resource: RawTemperature\n");
	}
	else
	{
	    value.uint16_v = MMODGENMSG_temp_get(&t);
	    bionet_resource_set(resource, &value, &tv);
	}

	resource = bionet_node_get_resource_by_id(node, "Photo");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get resource: Photo\n");
	}
	else
	{
	    value.uint16_v = mts310_cook_light(mv, MMODGENMSG_photo_get(&t));
	    bionet_resource_set(resource, &value, &tv);
	}

	resource = bionet_node_get_resource_by_id(node, "RawPhoto");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get resource: RawPhoto\n");
	}
	else
	{
	    value.uint16_v = MMODGENMSG_photo_get(&t);
	    bionet_resource_set(resource, &value, &tv);
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
		value.uint16_v = MMODGENMSG_accel_x_get(&t);
		bionet_resource_set(resource, &value, &tv);

		if ((last != 0) && (last + 1 != value.uint16_v))
		{
		    missed++;
		}
		last = value.uint16_v;
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
		value.float_v = mts310_cook_accel(MMODGENMSG_node_id_get(&t), 
						  X_AXIS,
						  MMODGENMSG_accel_x_get(&t));
		bionet_resource_set(resource, &value, &tv);
	    }
	
	    resource = bionet_node_get_resource_by_id(node, "RawAccel-X");
	    if (NULL == resource)
	    {
		fprintf(stderr, "Failed to get resource: RawAccel-X\n");
	    }
	    else
	    {
		value.uint16_v = MMODGENMSG_accel_x_get(&t);
		bionet_resource_set(resource, &value, &tv);
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
		value.uint16_v = MMODGENMSG_accel_y_get(&t);
		bionet_resource_set(resource, &value, &tv);

		if ((last != 0) && (last + 1 != value.uint16_v))
		{
		    missed++;
		    /* bump the missed message counter */
		    /* will be set in bionet later */
		}
		last = value.uint16_v;
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
		value.float_v = mts310_cook_accel(MMODGENMSG_node_id_get(&t), 
						  Y_AXIS,
						  MMODGENMSG_accel_y_get(&t));
		bionet_resource_set(resource, &value, &tv);
	    }
	    
	    resource = bionet_node_get_resource_by_id(node, "RawAccel-Y");
	    if (NULL == resource)
	    {
		fprintf(stderr, "Failed to get resource: RawAccel-Y\n");
	    }
	    else
	    {
		value.uint16_v = MMODGENMSG_accel_y_get(&t);
		bionet_resource_set(resource, &value, &tv);
	    }
	}

	resource = bionet_node_get_resource_by_id(node,
						  "MissedMsgs");
	if (NULL != resource)
	{
	    value.uint16_v = missed;
	    bionet_resource_set(resource, &value, &tv);
	}

#if DEBUG
	fprintf(stderr, "Reporting general update...\n");
#endif
	hab_report_datapoints(node);
    }

    return 0;
} /* msg_gen_process() */


int msg_accel_process(uint8_t *msg, ssize_t len)
{
    tmsg_t t = { msg, (size_t)len };
    bionet_node_t *node;
    char node_id[8];
    struct timeval tv;
    bionet_datapoint_value_t value;
    bionet_resource_t * resource;

#if DEBUG
    printf("    Message node: %04u\n", MMODACCELMSG_node_id_get(&t)); 
    printf("    Accel X:      %04u\n", MMODACCELMSG_accel_x_get(&t));
    printf("    Accel Y:      %04u\n", MMODACCELMSG_accel_y_get(&t));    
#endif /* DEBUG */

    /* if self is NULL then we haven't added any nodes yet, so return success */
    if (NULL == mmod_hab)
    {
	return 0;
    }

    snprintf(&node_id[0], 8, "%04u", MMODACCELMSG_node_id_get(&t));
    node = bionet_hab_get_node_by_id(mmod_hab, &node_id[0]);
    if (NULL == node)
    {
	/* node has not yet been found via general messages so ignore it */
#if DEBUG
	fprintf(stderr, "No node found to update.\n");
#endif /* DEBUG */
	return 0;
    }

    if (0 > gettimeofday(&tv, NULL))
    {
	g_warning("error with gettimeofday: %s", strerror(errno));
    }

    resource = bionet_node_get_resource_by_id(node, "Accel-X");
    if (NULL == resource)
    {
	fprintf(stderr, "Failed to get resource: Accel-X\n");
    }
    else
    {
	value.float_v = mts310_cook_accel(MMODGENMSG_node_id_get(&t), 
					  X_AXIS,
					  MMODGENMSG_accel_x_get(&t));
	bionet_resource_set(resource, &value, &tv);
    }

    resource = bionet_node_get_resource_by_id(node, "RawAccel-X");
    if (NULL == resource)
    {
	fprintf(stderr, "Failed to get resource: RawAccel-X\n");
    }
    else
    {
	value.float_v = MMODACCELMSG_accel_x_get(&t);
	bionet_resource_set(resource, &value, &tv);
    }

    resource = bionet_node_get_resource_by_id(node, "Accel-Y");
    if (NULL == resource)
    {
	fprintf(stderr, "Failed to get resource: Accel-Y\n");
    }
    else
    {
	value.float_v = mts310_cook_accel(MMODGENMSG_node_id_get(&t), 
					  Y_AXIS,
					  MMODGENMSG_accel_y_get(&t));
	bionet_resource_set(resource, &value, &tv);
    }

    resource = bionet_node_get_resource_by_id(node, "RawAccel-Y");
    if (NULL == resource)
    {
	fprintf(stderr, "Failed to get resource: RawAccel-Y\n");
    }
    else
    {
	value.float_v = MMODACCELMSG_accel_y_get(&t);
	bionet_resource_set(resource, &value, &tv);
    }


#if DEBUG
    fprintf(stderr, "Reporting accel update...\n");
#endif /* DEBUG */
    hab_report_datapoints(node);

    return 0;
} /* msg_accel_process() */


int msg_settings_process(uint8_t *msg, ssize_t len)
{
    char node_id[8];
    tmsg_t t = { msg, (size_t)len };
    bionet_node_t *node;
    bionet_resource_t *resource;
    struct timeval tv;
    bionet_datapoint_value_t value;

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
	return 0;
    }

    /* see if the settings resources have already been added to the node */
    resource = bionet_node_get_resource_by_id(node, "SampleInterval");
    if (NULL == resource)
    {
	/* resource doesn't exist yet, so ignore it */
	return 0;
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
	    value.uint16_v = MMODSETTINGSMSG_sample_interval_get(&t);
	    bionet_resource_set(resource, &value, &tv);
	}

	resource = bionet_node_get_resource_by_id(node, "NumAccelSamples");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get resource: NumAccelSamples\n");
	}
	else
	{
	    value.uint16_v = MMODSETTINGSMSG_num_accel_samples_get(&t);
	    bionet_resource_set(resource, &value, &tv);
	}

	resource = bionet_node_get_resource_by_id(node, "AccelSampleInterval");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get resource: AccelSampleInterval\n");
	}
	else
	{
	    value.uint16_v = MMODSETTINGSMSG_accel_sample_interval_get(&t);
	    bionet_resource_set(resource, &value, &tv);
	}

	resource = bionet_node_get_resource_by_id(node, "HeartbeatTime");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get resource: HeartbeatTime\n");
	}
	else
	{
	    value.uint16_v = MMODSETTINGSMSG_heartbeat_time_get(&t);
	    bionet_resource_set(resource, &value, &tv);
	}

	if (value.uint16_v < heartbeat_time)
	{
	    heartbeat_time = (uint32_t)value.uint16_v;
	}

	resource = bionet_node_get_resource_by_id(node, "AccelAxis");
	flags = MMODSETTINGSMSG_accel_flags_get(&t);
	if (flags & ACCEL_FLAG_X)
	{
	    if (flags & ACCEL_FLAG_Y)
	    {
		bionet_resource_set_with_valuestr(resource, "both", &tv);
	    }
	    else
	    {
		bionet_resource_set_with_valuestr(resource, "X-Axis", &tv);
	    }
	}
	else if (flags & ACCEL_FLAG_Y)
	{
	    bionet_resource_set_with_valuestr(resource, "Y-Axis", &tv);
	}
	else
	{
	    bionet_resource_set_with_valuestr(resource, "None", &tv);
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
