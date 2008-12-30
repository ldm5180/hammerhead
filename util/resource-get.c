
//
// Copyright (C) 2008, Regents of the University of Colorado.
//

#include "bionet-util.h"
#include "internal.h"


bionet_node_t * bionet_resource_get_node(const bionet_resource_t *resource)
{
    return resource->node;
} /* bionet_resource_get_node() */


bionet_resource_data_type_t bionet_resource_get_data_type(const bionet_resource_t *resource)
{
    return resource->data_type;
} /* bionet_resource_get_data_type() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
