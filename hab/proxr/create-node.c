#include "sim-hab.h"


//
// create node
//
void add_node(bionet_hab_t *hab, char *name)
{
    bionet_node_t *node;

    // add new node
    node = bionet_node_new(hab, name);

    // add potentiometer resources
    add_pot_resource(node, "pot-00\0");
    add_pot_resource(node, "pot-01\0");
    add_pot_resource(node, "pot-02\0");
    add_pot_resource(node, "pot-03\0");
    add_pot_resource(node, "pot-04\0");
    add_pot_resource(node, "pot-05\0");
    add_pot_resource(node, "pot-06\0");
    add_pot_resource(node, "pot-07\0");
    add_pot_resource(node, "pot-08\0");
    add_pot_resource(node, "pot-09\0");
    add_pot_resource(node, "pot-10\0");
    add_pot_resource(node, "pot-11\0");
    add_pot_resource(node, "pot-12\0");
    add_pot_resource(node, "pot-13\0");
    add_pot_resource(node, "pot-14\0");
    add_pot_resource(node, "pot-15\0");

    bionet_hab_add_node(hab, node);

    hab_report_new_node(node);
}
