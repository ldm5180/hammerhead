
// Copyright (c) 2008-2010, Regents of the University of Colorado.
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

int bionet_datapoint_cmp(const void *d_a, const void *d_b) {
    int r;
    const bionet_datapoint_t * a = d_a;
    const bionet_datapoint_t * b = d_b;

    r = bionet_cmp_resource(a->value->resource, b->value->resource);
    if(r) return r;
    
    r = b->timestamp.tv_sec - a->timestamp.tv_sec;
    if(r) return r;

    r = b->timestamp.tv_usec - a->timestamp.tv_usec;
    if(r) return r;


    switch(a->value->resource->data_type){
        case BIONET_RESOURCE_DATA_TYPE_INVALID:
            return 0;
        case BIONET_RESOURCE_DATA_TYPE_BINARY:
            return b->value->content.binary_v - a->value->content.binary_v;
        case BIONET_RESOURCE_DATA_TYPE_UINT8:
            return b->value->content.uint8_v - a->value->content.uint8_v;
        case BIONET_RESOURCE_DATA_TYPE_INT8:
            return b->value->content.int8_v - a->value->content.int8_v;
        case BIONET_RESOURCE_DATA_TYPE_UINT16:
            return b->value->content.uint16_v - a->value->content.uint16_v;
        case BIONET_RESOURCE_DATA_TYPE_INT16:
            return b->value->content.int16_v - a->value->content.int16_v;
        case BIONET_RESOURCE_DATA_TYPE_UINT32:
            return b->value->content.uint32_v - a->value->content.uint32_v;
        case BIONET_RESOURCE_DATA_TYPE_INT32:
            return b->value->content.int32_v - a->value->content.int32_v;
        case BIONET_RESOURCE_DATA_TYPE_FLOAT:
            if(a->value->content.float_v == b->value->content.float_v) return 0;
            if(a->value->content.float_v < b->value->content.float_v) return -1;
            return 1;
        case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
            if(a->value->content.double_v == b->value->content.double_v) return 0;
            if(a->value->content.double_v < b->value->content.double_v) return -1;
            return 1;
        case BIONET_RESOURCE_DATA_TYPE_STRING:
            return strcmp(a->value->content.string_v, b->value->content.string_v);
    }

    return 0;

}

int bionet_datapoint_iseq(const bionet_datapoint_t *dp1, const bionet_datapoint_t *dp2) {
    return bionet_datapoint_cmp(dp1, dp2);
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
