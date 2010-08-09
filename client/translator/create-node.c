#include "translator.h"

void create_node(bionet_hab_t *hab, char *name)
{
    bionet_node_t *node;
    
    node = bionet_node_new(hab, name);

    create_resource(node, "adc-00\0");
    create_resource(node, "adc-01\0");
    create_resource(node, "adc-02\0");
    create_resource(node, "adc-03\0");
    create_resource(node, "adc-04\0");
    create_resource(node, "adc-05\0");
    create_resource(node, "adc-06\0");
    create_resource(node, "adc-07\0");
    create_resource(node, "adc-08\0");
    create_resource(node, "adc-09\0");
    create_resource(node, "adc-10\0");
    create_resource(node, "adc-11\0");
    create_resource(node, "adc-12\0");
    create_resource(node, "adc-13\0");
    create_resource(node, "adc-14\0");
    create_resource(node, "adc-15\0");
    
    bionet_hab_add_node(hab, node);

    hab_report_new_node(node);
}
