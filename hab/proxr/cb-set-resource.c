#include <string.h>
#include "proxrcmds.h"
#include "sim-hab.h"

void cb_set_resource(bionet_resource_t *resource, bionet_value_t *value)
{
    uint8_t content;
    int id;
    bionet_node_t *node;

    bionet_value_get_uint8(value, &content);

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

