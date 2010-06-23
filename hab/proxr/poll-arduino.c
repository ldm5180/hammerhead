#include "sim-hab.h"
#include <stdlib.h>

int poll_arduino()
{
    char response[256];
    uint32_t content;
    bionet_resource_t *res;
    bionet_node_t *node;
    
    node = bionet_hab_get_node_by_index(hab, 0);

    // send command 100. requests analog0's value
    // read value and set resource
    arduino_write(100);
    arduino_read_until(response, '\n');
    content = atoi(response); 
    res = bionet_node_get_resource_by_index(node, 24);
    bionet_resource_set_uint32(res, content, NULL);


    // send command 101. requests analog1's value
    // read value and set resource
    arduino_write(101);
    arduino_read_until(response, '\n');
    content = atoi(response);
    res = bionet_node_get_resource_by_index(node, 25);
    bionet_resource_set_uint32(res, content, NULL);

    // send command 200. requests the 8 digital values
    // read values and set resource
    arduino_write(200);
    arduino_read_until(response, '\n');
    for(int i=0; i<8; i++)
    {
        content = atoi(&response[i]);
        res = bionet_node_get_resource_by_index(node, 16+i);
        bionet_resource_set_binary(res, content, NULL);
    }

    // report new data
    hab_report_datapoints(node);

    return 1;
}  
    

    
