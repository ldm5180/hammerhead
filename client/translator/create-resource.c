#include "translator.h"
#include <string.h>

bionet_resource_t* create_adc_resource(bionet_node_t *node, char *name)
{
    bionet_resource_t *resource;
    bionet_value_t *value;
    bionet_datapoint_t *datapoint;
    int r;

    resource = bionet_resource_new(
        node,
        BIONET_RESOURCE_DATA_TYPE_DOUBLE,
        BIONET_RESOURCE_FLAVOR_ACTUATOR,
        name);
    
    if(resource == NULL)
    {
        fprintf(stderr, "Error creating Resource for resource %s\n", name);
        return NULL;
    }

    r = bionet_node_add_resource(node, resource);
    if(r != 0)
    {
        fprintf(stderr, "Error adding Resource for resource %s\n", name);
        return NULL;
    }

    value = bionet_value_new_double(resource, 0);
    if(value == NULL)
    {
        fprintf(stderr, "Error creating value for resource %s\n", name);
        return NULL;
    }

    datapoint = bionet_datapoint_new(resource, value, NULL);
    bionet_resource_add_datapoint(resource, datapoint);

    return resource;
}

bionet_resource_t* create_range_resource(bionet_node_t *node, char *name)
{
    bionet_resource_t *resource;
    bionet_value_t *value;
    bionet_datapoint_t *datapoint;
    int r;

    resource = bionet_resource_new(
        node,
        BIONET_RESOURCE_DATA_TYPE_DOUBLE,
        BIONET_RESOURCE_FLAVOR_SENSOR,
        name);
    
    if(resource == NULL)
    {
        fprintf(stderr, "Error creating Resource for resource %s\n", name);
        return NULL;
    }

    r = bionet_node_add_resource(node, resource);
    if(r != 0)
    {
        fprintf(stderr, "Error adding Resource for resource %s\n", name);
        return NULL;
    }

    value = bionet_value_new_double(resource, 0);
    if(value == NULL)
    {
        fprintf(stderr, "Error creating value for resource %s\n", name);
        return NULL;
    }

    datapoint = bionet_datapoint_new(resource, value, NULL);
    bionet_resource_add_datapoint(resource, datapoint);

    return resource;
}

bionet_resource_t* create_state_resource(bionet_node_t *node, char *name)
{
    bionet_resource_t *resource;
    bionet_value_t *value;
    bionet_datapoint_t *datapoint;
    int r;

    resource = bionet_resource_new(
        node,
        BIONET_RESOURCE_DATA_TYPE_INT8,
        BIONET_RESOURCE_FLAVOR_SENSOR,
        name);
    
    if(resource == NULL)
    {
        fprintf(stderr, "Error creating Resource for resource %s\n", name);
        return NULL;
    }

    r = bionet_node_add_resource(node, resource);
    if(r != 0)
    {
        fprintf(stderr, "Error adding Resource for resource %s\n", name);
        return NULL;
    }

    value = bionet_value_new_int8(resource, 0);
    if(value == NULL)
    {
        fprintf(stderr, "Error creating value for resource %s\n", name);
        return NULL;
    }

    datapoint = bionet_datapoint_new(resource, value, NULL);
    bionet_resource_add_datapoint(resource, datapoint);

    return resource;
}


