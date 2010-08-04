#include <string.h>
#include "proxrcmds.h"
#include "sim-hab.h"

void cb_set_resource(bionet_resource_t *resource, bionet_value_t *value)
{
    double data;
    double content;
    int id;
    bionet_node_t *node;

    bionet_value_get_float(value, &data);
    if(data < 0 || data > 255)
	return;

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
    	    // command proxr to adjust to new value
    	    set_potentiometer(id, (int)data); 
	    // set resources datapoint to new value
            content = data*POT_CONVERSION; 
            bionet_resource_set_double(resource, content, NULL);
            hab_report_datapoints(node);
            return;
        }
    }
}

