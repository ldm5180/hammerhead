
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "bionet-util.h"




bionet_resource_t *bionet_resource_new(
    const bionet_node_t *node,
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


bionet_resource_t *bionet_resource_new_from_str(
    const bionet_node_t *node,
    const char *data_type_str,
    const char *flavor_str,
    const char *id
) {
    bionet_resource_data_type_t data_type;
    bionet_resource_flavor_t flavor;

    // sanity checking
    if (flavor_str == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_new_from_str(): NULL Resource-Flavor passed in");
        return NULL;
    }
    if (data_type_str == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_new_from_str(): NULL Resource-Data-Type passed in");
        return NULL;
    }

    // parse the Data Type and Flavor
    data_type = bionet_resource_data_type_from_string(data_type_str);
    if (data_type == BIONET_RESOURCE_DATA_TYPE_INVALID) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_new_from_str(): error parsing data type from '%s'", data_type_str);
        return NULL;
    }
    flavor = bionet_resource_flavor_from_string(flavor_str);
    if (flavor == BIONET_RESOURCE_FLAVOR_INVALID) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_new_from_str(): error parsing flavor from '%s'", flavor_str);
        return NULL;
    }

    // looks good, make the resource & return it
    return bionet_resource_new(node, data_type, flavor, id);
}

