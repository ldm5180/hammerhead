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
    for(int i=0; i<16; i++)
    {
        add_pot_resource(node, i);
    }

    bionet_hab_add_node(hab, node);

    hab_report_new_node(node);
}
