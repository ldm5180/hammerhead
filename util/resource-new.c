
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <glib.h>

#include "bionet-util.h"
#include "internal.h"



bionet_resource_t *bionet_resource_new(
    bionet_node_t *node,
    bionet_resource_data_type_t data_type,
    bionet_resource_flavor_t flavor,
    const char *id
) {
    bionet_resource_t *resource;

    // sanity checking
    if (id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_new(): NULL Resource-ID passed in");
        return NULL;
    }

    if (
        (data_type < BIONET_RESOURCE_DATA_TYPE_MIN) ||
        (data_type > BIONET_RESOURCE_DATA_TYPE_MAX)
    ) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_new(): invalid data type %d", data_type);
        return NULL;
    }

    if (
        (flavor < BIONET_RESOURCE_FLAVOR_MIN) ||
        (flavor > BIONET_RESOURCE_FLAVOR_MAX)
    ) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_new(): invalid flavor %d", flavor);
        return NULL;
    }


    resource = (bionet_resource_t *)calloc(1, sizeof(bionet_resource_t));
    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "out of memory!");
        return NULL;
    }

    resource->datapoints = g_ptr_array_new();

    resource->node = node;

    resource->id = strdup(id);
    if (resource->id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "out of memory!");
        goto cleanup;
    }

    resource->data_type = data_type;
    resource->flavor = flavor;

    return resource;


cleanup:
    if (resource != NULL) {
        if (resource->datapoints != NULL) {
            g_ptr_array_free(resource->datapoints, TRUE);
        }
        if (resource->id != NULL) {
            free(resource->id);
        }
        bionet_resource_free(resource);
    }
    return NULL;
}


const char *bionet_resource_get_id(bionet_resource_t *resource)
{
    if (NULL == resource)
    {
	errno = EINVAL;
	return NULL;
    }

    return resource->id;
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
