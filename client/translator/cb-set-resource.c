#include "translator.h"
#include <stdlib.h>

void cb_set_resource(bionet_resource_t *resource, bionet_value_t *value)
{
    double content, voltage;
    unsigned long int adc;
    char id[9];
    char *name = NULL;
    bionet_node_t *node;

    node = bionet_resource_get_node(resource);
    bionet_value_get_double(value, &content);

    // get resource name
    bionet_split_resource_name(bionet_resource_get_name(resource), NULL, NULL, NULL, &name);    

    // get adc number from resource name
    adc = (unsigned long int)g_hash_table_lookup(hash_table, name);
    printf("mapping %s to %ld.\n", name, adc);

    // find corresponding voltage from engineering value
    voltage = find_voltage(adc, content);
    if(voltage == -1)
    {
        printf("could not find appropiate value\n");
        return;
    }

    // command proxr hab to voltage
    // proxr hab works with 8 bit resolution
    double v = voltage/1000;
    sprintf(id, "%f", v);
    bionet_set_resource(proxr_resource[adc], id);

    bionet_resource_set_double(resource, content, NULL);
    hab_report_datapoints(node);
}
