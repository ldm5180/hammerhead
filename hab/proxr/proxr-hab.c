#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "hardware-abstractor.h"
#include "glib.h"

#include "proxrcmds.h"
#include "proxrport.h"

#define HAB_TYPE "proxr-hab"

bionet_hab_t *hab;
int should_exit = 0;

void cb_set_resource(bionet_resource_t *resource, bionet_value_t *value)
{
    uint8_t content;
    int id;
    bionet_node_t *node;

    bionet_value_get_uint8(value, &content);
    /*if(content < 0 || content > 255)
    {
        g_warning("Invalid value. Valid range 0-255.");
        return;
    }*/
    node = bionet_resource_get_node(resource);
    // get index of resource
    //FIXME: probably a better way to do this
    for(int i=0; i<16; i++)
    {
        char buf[5];
        char name[24];
        strcpy(name, "Potentiometer\0");
        sprintf(buf,"%d", i);
        int len = strlen(buf);
        buf[len] = '\0';
        strcat(name, buf);

        if(bionet_resource_matches_id(resource, name))
        {
            id = i;
            break;
        }
    }
    // command proxr to adjust to new value
    set_potentiometer(id, (int)content); 
    // set resources datapoint to new value
    bionet_resource_set(resource, value, NULL);
    hab_report_datapoints(bionet_resource_get_node(resource));
}

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
        BIONET_RESOURCE_DATA_TYPE_UINT8,
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

    value = bionet_value_new_uint8(resource, 0);
    if(value == NULL)
    {
        fprintf(stderr, "Error creating value for resource %d\n", id);
        return;
    }

    datapoint = bionet_datapoint_new(resource, value, NULL);
    bionet_resource_add_datapoint(resource, datapoint);


}

void add_do_resource(bionet_node_t *node, int id)
{ 
    bionet_resource_t *resource;
    bionet_value_t *value;
    bionet_datapoint_t *datapoint;
    int r;

    char buf[5];
    char name[24];
    strcpy(name, "DO\0");
    sprintf(buf,"%d", id);
    int i = strlen(buf);
    buf[i] = '\0';
    strcat(name, buf);

    resource = bionet_resource_new(
        node,
        BIONET_RESOURCE_DATA_TYPE_BINARY,
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

    value = bionet_value_new_binary(resource, 0);
    if(value == NULL)
    {
        fprintf(stderr, "Error creating value for resource %d\n", id);
        return;
    }

    datapoint = bionet_datapoint_new(resource, value, NULL);
    bionet_resource_add_datapoint(resource, datapoint);


}
void add_node(bionet_hab_t *hab, char *name)
{
    bionet_node_t *node;

    // add new node
    node = bionet_node_new(hab, name);

    // add potentiometer resources
    for(int i=0; i<16; i++)
    {
        add_pot_resource(node, i);
    }

    // add digital out resources
    for(int i=0; i<8; i++)
    {
        add_do_resource(node, i);
    }

    bionet_hab_add_node(hab, node);

    hab_report_new_node(node);
}

void signal_handler(int unused)
{
    printf("\n");
    g_message("Exiting...");
    should_exit = 1;
}

int main(int argc, char* argv[])
{
    int bionet_fd;
    int proxr_fd;
    char *hab_type = HAB_TYPE;
    char *hab_id = NULL;

    hab = bionet_hab_new(hab_type, hab_id);

    // register callbacks
    hab_register_callback_set_resource(cb_set_resource);

    // connect to bionet
    bionet_fd = hab_connect(hab);
    if(bionet_fd < 0)
    {
        fprintf(stderr, "Error connecting to Bionet, exiting.\n");
        return 1;
    }

    g_message("%s connected to Bionet!", bionet_hab_get_name(hab));

    // connect to proxr controller
    proxr_fd = open_port();
    if(proxr_fd < 0)
    {
        g_warning("could not connect to proxr device, exiting");
        return 1;
    }

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // add node
    add_node(hab, "sim1");
    set_all_potentiometers(0);

    //
    // main loop
    //
    while(1)
    {
        hab_read_with_timeout(NULL);

        if(should_exit)
        {
            hab_disconnect();
            close_port();
            break;
        }
    }

    return 0;
}
       


