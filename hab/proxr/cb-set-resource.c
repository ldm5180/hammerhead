#include <string.h>
#include <stdlib.h>
#include "proxrcmds.h"
#include "sim-hab.h"

void cb_set_resource(bionet_resource_t *resource, bionet_value_t *value)
{
    double data;
    double content;
    char number[3];
    char *res_name = NULL;
    long int id;
    bionet_node_t *node;

    bionet_value_get_double(value, &data);
    if(data < 0)
        data = 0;
    if(data > 5)
        data = 5;

    node = bionet_resource_get_node(resource);
    bionet_split_resource_name(bionet_resource_get_name(resource), NULL, NULL, NULL, &res_name);

    // extract pot number from resource name
    number[0] = res_name[4];
    number[1] = res_name[5];
    number[2] = '\0';
    id = strtol(number, NULL, 10);
   
    // proxr hardware works with 8 bit resolution 0-255 steps 
    data = data/POT_CONVERSION;
    // command proxr to adjust to new value
    set_potentiometer(id, (int)data);
    // set resources datapoint to new value and report
    content = data*POT_CONVERSION;
    bionet_resource_set_double(resource, content, NULL);
    hab_report_datapoints(node);
}
   
