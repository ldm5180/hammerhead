
//
// Copyright (C) 2008, Regents of the University of Colorado.
//

#include <errno.h>

#include "bionet-util.h"
#include "internal.h"


bionet_node_t * bionet_resource_get_node(const bionet_resource_t *resource)
{
    if (NULL == resource)
    {
	errno = EINVAL;
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_node(): passed in NULL resource");
	return NULL;
    }

    return resource->node;
} /* bionet_resource_get_node() */


bionet_hab_t * bionet_resource_get_hab(const bionet_resource_t *resource)
{
    if ((NULL == resource) || (NULL == resource->node))
    {
	errno = EINVAL;
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_hab(): passed in NULL resource or node");
	return NULL;
    }

    return resource->node->hab;
} /* bionet_resource_get_hab() */


bionet_resource_data_type_t bionet_resource_get_data_type(const bionet_resource_t *resource)
{
    if (NULL == resource)
    {
	errno = EINVAL;
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_data_type(): passed in NULL resource");
	return BIONET_RESOURCE_DATA_TYPE_INVALID;
    }

    return resource->data_type;
} /* bionet_resource_get_data_type() */


bionet_resource_flavor_t bionet_resource_get_flavor(const bionet_resource_t *resource)
{
    if (NULL == resource)
    {
	errno = EINVAL;
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_flavor(): passed in NULL resource");
	return BIONET_RESOURCE_FLAVOR_INVALID;
    }

    return resource->flavor;
} /* bionet_resource_get_flavor() */

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
