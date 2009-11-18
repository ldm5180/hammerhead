
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#define _GNU_SOURCE  // for strtof(3)

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "bionet-util.h"
#include "internal.h"


void bionet_datapoint_set_value(bionet_datapoint_t *datapoint, bionet_value_t *value) {
    if (datapoint == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_set_value(): NULL datapoint passed in");
        return;
    }
    if (value == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_set_value(): NULL value passed in");
        return;
    }

    if (value == datapoint->value)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_set_value(): passed in value == d->value");
	datapoint->dirty = 1;
	return;
    }
    else if (datapoint->value) {
        bionet_value_free(datapoint->value);
	datapoint->value = value;
    } else {
        datapoint->value = value;
    }

    datapoint->dirty = 1;
}


bionet_value_t * bionet_datapoint_get_value(bionet_datapoint_t *datapoint)
{
    if (NULL == datapoint)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_datapoint_get_value(): NULL datapoint passed in");
	errno = EINVAL;
	return NULL;
    }

    return datapoint->value;
} /* bionet_datapoint_get_value() */


bionet_resource_t * bionet_datapoint_get_resource(const bionet_datapoint_t * datapoint)
{
    if (NULL == datapoint)
    {
	errno = EINVAL;
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_get_resource(): passed in NULL datapoint");
	return NULL;
    }

    if (NULL == datapoint->value) 
    {
	//not an error, datapoints aren't always part of values
	return NULL;
    }

    return datapoint->value->resource;
} /* bionet_datapoint_get_resource() */


bionet_node_t * bionet_datapoint_get_node(const bionet_datapoint_t * datapoint)
{
    if (NULL == datapoint)
    {
	errno = EINVAL;
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_get_node(): passed in NULL datapoint");
	return NULL;
    }

    if ((NULL == datapoint->value) || (NULL == datapoint->value->resource))
    {
	//not an error, datapoints aren't always part of values or resource
	return NULL;
    }

    return datapoint->value->resource->node;
} /* bionet_datapoint_get_node() */


bionet_hab_t * bionet_datapoint_get_hab(const bionet_datapoint_t * datapoint)
{
    if (NULL == datapoint)
    {
	errno = EINVAL;
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_get_hab(): passed in NULL datapoint");
	return NULL;
    }

    if ((NULL == datapoint->value) || (NULL == datapoint->value->resource))
    {
	//not an error, datapoints aren't always part of values or resource
	return NULL;
    }

    if (NULL == datapoint->value->resource->node)
    {
	errno = EINVAL;
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_get_hab(): passed in datapoint has NULL node ptr");
	return NULL;
    }

    return datapoint->value->resource->node->hab;
} /* bionet_datapoint_get_hab() */


int bionet_datapoint_iseq(const bionet_datapoint_t *dp1, const bionet_datapoint_t *dp2) {
    if(dp1->timestamp.tv_sec != dp2->timestamp.tv_sec) return 1;
    if(dp1->timestamp.tv_usec != dp2->timestamp.tv_usec) return 1;

    if(dp1->value->resource->data_type != dp2->value->resource->data_type) return 1;

    switch(dp1->value->resource->data_type){
        case BIONET_RESOURCE_DATA_TYPE_BINARY:
            if ( dp1->value->content.binary_v != dp2->value->content.binary_v) return 1;
            break;

        case BIONET_RESOURCE_DATA_TYPE_UINT8:
            if ( dp1->value->content.uint8_v != dp2->value->content.uint8_v) return 1;
            break;

        case BIONET_RESOURCE_DATA_TYPE_INT8:
            if ( dp1->value->content.int8_v != dp2->value->content.int8_v) return 1;
            break;

        case BIONET_RESOURCE_DATA_TYPE_UINT16:
            if ( dp1->value->content.uint16_v != dp2->value->content.uint16_v) return 1;
            break;

        case BIONET_RESOURCE_DATA_TYPE_INT16:
            if ( dp1->value->content.int16_v != dp2->value->content.int16_v) return 1;
            break;

        case BIONET_RESOURCE_DATA_TYPE_UINT32:
            if ( dp1->value->content.uint32_v != dp2->value->content.uint32_v) return 1;
            break;

        case BIONET_RESOURCE_DATA_TYPE_INT32:
            if ( dp1->value->content.int32_v != dp2->value->content.int32_v) return 1;
            break;

        case BIONET_RESOURCE_DATA_TYPE_FLOAT:
            if ( dp1->value->content.float_v != dp2->value->content.float_v) return 1;
            break;

        case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
            if ( dp1->value->content.double_v != dp2->value->content.double_v) return 1;
            break;

        case BIONET_RESOURCE_DATA_TYPE_STRING:
            if ( strcmp(dp1->value->content.string_v, dp2->value->content.string_v) ) return 1;
            break;

        default:
            return 1;
    }

    return 0;

}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
