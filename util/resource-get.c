
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
	return NULL;
    }

    return resource->node;
} /* bionet_resource_get_node() */


bionet_resource_data_type_t bionet_resource_get_data_type(const bionet_resource_t *resource)
{
    if (NULL == resource)
    {
	errno = EINVAL;
	return BIONET_RESOURCE_DATA_TYPE_INVALID;
    }

    return resource->data_type;
} /* bionet_resource_get_data_type() */


bionet_resource_flavor_t bionet_resource_get_flavor(const bionet_resource_t *resource)
{
    if (NULL == resource)
    {
	errno = EINVAL;
	return BIONET_RESOURCE_FLAVOR_INVALID;
    }

    return resource->flavor;
} /* bionet_resource_get_flavor() */

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
