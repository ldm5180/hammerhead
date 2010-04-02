
// Copyright (c) 2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "env.h"
#include "hardware-abstractor.h"
#include "env-message.h"
#include "envgenmsg.h"
#include "message.h"
#include "bionet-hab.h"
#include "mts310_cook.h"
#include "serialsource.h"
#include "message.h"

extern bionet_hab_t * env_hab;

int msg_gen_process(uint8_t *msg, ssize_t len)
{
    tmsg_t t = { msg, (size_t)len };
    bionet_node_t *node;
    char node_id[8];
    struct timeval tv;
    uint32_t mv = 0;
    bionet_resource_t * resource;

#if DEBUG
    printf("    Message node: %04u\n", ENVGENMSG_node_id_get(&t)); 
    printf("    Voltage:      %04u\n", ENVGENMSG_volt_get(&t));
    printf("    Temperature:  %04u\n", ENVGENMSG_temp_get(&t));
    printf("    Photo:        %04u\n", ENVGENMSG_photo_get(&t));
#endif /* DEBUG */

    snprintf(&node_id[0], 8, "%04u", ENVGENMSG_node_id_get(&t));
    node = bionet_hab_get_node_by_id(env_hab, &node_id[0]);

    if (0 > gettimeofday(&tv, NULL))
    {
	g_warning("error with gettimeofday: %s", strerror(errno));
    }

    /* if this node doesn't exist yet, create it */
    if (NULL == node)
    {

#if DEBUG
	fprintf(stderr, "Creating new node %s\n", &node_id[0]);
#endif
	node = bionet_node_new(env_hab, &node_id[0]);
	if (NULL == node)
	{
	    fprintf(stderr, "Failed to create new node\n");
	    return 1;
	}

	if (bionet_hab_add_node(env_hab, node)) {
	    g_log("", G_LOG_LEVEL_WARNING, "msg_gen_process(): Failed to add node to hab.");
	    bionet_node_free(node);
	    return 1;
	}

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
	    mv = mts310_cook_voltage(ENVGENMSG_volt_get(&t));
	    if (bionet_resource_set_uint32(resource, mv, &tv))
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
	    if (bionet_resource_set_float(resource, mts310_cook_temperature(ENVGENMSG_temp_get(&t)), &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
	    }
	}

	/* create temperature resource */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_FLOAT,
				       BIONET_RESOURCE_FLAVOR_SENSOR, 
				       "Temperature-F");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get new resource: Temperature-F\n");
	}
	else
	{
	    if (bionet_node_add_resource(node, resource))
	    {
		fprintf(stderr, "Failed to add resource: Temperature-F\n");
	    }
	    float degrees = mts310_cook_temperature(ENVGENMSG_temp_get(&t));
	    degrees = (degrees * (9.0/5.0))+32.0;
	    if (bionet_resource_set_float(resource, degrees, &tv))
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
	    if (bionet_resource_set_uint16(resource, mts310_cook_light(mv, ENVGENMSG_photo_get(&t)), &tv))
	    {
		fprintf(stderr, "Failed to set resource\n"); 
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
	mv = mts310_cook_voltage(ENVGENMSG_volt_get(&t));
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get resource: Voltage\n");
	}
	else
	{
	    bionet_resource_set_uint32(resource, mv, &tv);
	}

	resource = bionet_node_get_resource_by_id(node, "Temperature");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get resource: Temperature\n");
	}
	else
	{
	    bionet_resource_set_float(resource, mts310_cook_temperature(ENVGENMSG_temp_get(&t)), &tv);
	}

	resource = bionet_node_get_resource_by_id(node, "Temperature-F");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get resource: Temperature-F\n");
	}
	else
	{
	    float degrees = mts310_cook_temperature(ENVGENMSG_temp_get(&t));
	    degrees = (degrees * (9.0/5.0))+32.0;
	    bionet_resource_set_float(resource, degrees, &tv);
	}

	resource = bionet_node_get_resource_by_id(node, "Photo");
	if (NULL == resource)
	{
	    fprintf(stderr, "Failed to get resource: Photo\n");
	}
	else
	{
	    bionet_resource_set_uint16(resource, mts310_cook_light(mv, ENVGENMSG_photo_get(&t)), &tv);
	}

#if DEBUG
	fprintf(stderr, "Reporting general update...\n");
#endif
	hab_report_datapoints(node);
    }

    return 0;
} /* msg_gen_process() */

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
