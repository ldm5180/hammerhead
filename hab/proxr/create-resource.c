#include <string.h>
#include "sim-hab.h"


//
// create potentiometer resources
//
void add_pot_resource(bionet_node_t *node, int id)
{ 
    bionet_resource_t *resource;
    bionet_value_t *value;
    bionet_datapoint_t *datapoint;
    int r;

    char buf[5];
    char name[24];
    strcpy(name, "Potentiometer\0");
    sprintf(buf,"%d", id);
    int i = strlen(buf);
    buf[i] = '\0';
    strcat(name, buf);

    resource = bionet_resource_new(
        node,
        BIONET_RESOURCE_DATA_TYPE_FLOAT,
        BIONET_RESOURCE_FLAVOR_ACTUATOR,
        name);
    if(resource == NULL)
    {
        fprintf(stderr, "Error creating Resource for resource %d\n", id);
        return;
    }

    r = bionet_node_add_resource(node, resource);
    if(r != 0)
    {
        fprintf(stderr, "Error adding Resource for resource %d\n", id);
        return;
    }

    value = bionet_value_new_float(resource, 0);
    if(value == NULL)
    {
        fprintf(stderr, "Error creating value for resource %d\n", id);
        return;
    }

    datapoint = bionet_datapoint_new(resource, value, NULL);
    bionet_resource_add_datapoint(resource, datapoint);
}

//
// create digital out resources
//
void add_di_resource(bionet_node_t *node, int id)
{ 
    bionet_resource_t *resource;
    bionet_value_t *value;
    bionet_datapoint_t *datapoint;
    int r;

    char buf[5];
    char name[24];
    strcpy(name, "digital-input\0");
    sprintf(buf,"%d", id);
    int i = strlen(buf);
    buf[i] = '\0';
    strcat(name, buf);

    resource = bionet_resource_new(
        node,
        BIONET_RESOURCE_DATA_TYPE_BINARY,
        BIONET_RESOURCE_FLAVOR_SENSOR,
        name);
    if(resource == NULL)
    {
        fprintf(stderr, "Error creating Resource for resource %d\n", id);
        return;
    }

    r = bionet_node_add_resource(node, resource);
    if(r != 0)
    {
        fprintf(stderr, "Error adding Resource for resource %d\n", id);
        return;
    }

    value = bionet_value_new_binary(resource, 0);
    if(value == NULL)
    {
        fprintf(stderr, "Error creating value for resource %d\n", id);
        return;
    }

    datapoint = bionet_datapoint_new(resource, value, NULL);
    bionet_resource_add_datapoint(resource, datapoint);
}

//
// create analog input resources
//
void add_ai_resource(bionet_node_t *node, int id)
{ 
    bionet_resource_t *resource;
    bionet_value_t *value;
    bionet_datapoint_t *datapoint;
    int r;

    char buf[5];
    char name[24];
    strcpy(name, "AnalogIn\0");
    sprintf(buf,"%d", id);
    int i = strlen(buf);
    buf[i] = '\0';
    strcat(name, buf);

    resource = bionet_resource_new(
        node,
        BIONET_RESOURCE_DATA_TYPE_UINT32,
        BIONET_RESOURCE_FLAVOR_PARAMETER,
        name);
    if(resource == NULL)
    {
        fprintf(stderr, "Error creating Resource for resource %d\n", id);
        return;
    }

    r = bionet_node_add_resource(node, resource);
    if(r != 0)
    {
        fprintf(stderr, "Error adding Resource for resource %d\n", id);
        return;
    }

    value = bionet_value_new_uint32(resource, 0);
    if(value == NULL)
    {
        fprintf(stderr, "Error creating value for resource %d\n", id);
        return;
    }

    datapoint = bionet_datapoint_new(resource, value, NULL);
    bionet_resource_add_datapoint(resource, datapoint);
}
