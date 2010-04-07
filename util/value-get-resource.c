
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "errno.h"

#include "bionet-util.h"
#include "internal.h"


bionet_resource_t * bionet_value_get_resource(bionet_value_t * value)
{
    if (NULL == value)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_value_get_resource(): NULL value passed in");
	errno = EINVAL;
	return NULL;
    }

    return value->resource;
} /* bionet_value_get_resource() */


bionet_node_t * bionet_value_get_node(const bionet_value_t * value)
{
    if ((NULL == value) || (NULL == value->resource))
    {
	errno = EINVAL;
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_value_get_node(): passed in NULL value or resource");
	return NULL;
    }

    return value->resource->node;
} /* bionet_value_get_node() */


bionet_hab_t * bionet_value_get_hab(const bionet_value_t * value)
{
    if ((NULL == value) || (NULL == value->resource) || (NULL == value->resource->node))
    {
	errno = EINVAL;
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_value_get_hab(): passed in NULL value or resource or node");
	return NULL;
    }

    return value->resource->node->hab;
} /* bionet_value_get_hab() */


bionet_datapoint_t * bionet_value_get_datapoint(const bionet_value_t * value)
{
    int i;
    bionet_datapoint_t *dp;

    if ((NULL == value) || (NULL == value->resource))

    {
	errno = EINVAL;
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_value_get_datapoint(): passed in NULL value or resource");
	return NULL;
    }

    if (0 == bionet_resource_get_num_datapoints(value->resource))
    {
	errno = EINVAL;
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_value_get_datapoint(): resource has 0 datapoints");
	return NULL;
    }

    //check all the datapoints on the resource
    for (i = 0; i < bionet_resource_get_num_datapoints(value->resource); i++)
    {
	dp = bionet_resource_get_datapoint_by_index(value->resource, i);
	if (NULL == dp)
	{
	    errno = EINVAL;
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_value_get_datapoint(): failed to get datapoint %d", i);
	    return NULL;
	}

	//if this has the value we asked for, return it
	if (dp->value == value)
	{
	    return dp;
	}
    }

    //must not have been found
    return NULL;
} /* bionet_value_get_datapoint() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
