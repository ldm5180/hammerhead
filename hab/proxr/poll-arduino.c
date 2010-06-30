#include "sim-hab.h"
#include <stdlib.h>

int poll_arduino()
{
    printf("version 1.1\n");
    char response[256];
    int data, err;
    float content;
    int oldVal;
    bionet_resource_t *res;
    bionet_node_t *node;
    
    node = bionet_hab_get_node_by_index(hab, 0);

    // send command 100. requests analog0's value
    // read value and set resource
    arduino_write(100);
    printf("\nanalog1\n");
    err = arduino_read_until(response, 'x');
    if(err == 0)
    {
        printf("%s\n", response);
        data = atoi(response);
        content = data*ANALOG_INPUT_CONVERSION;
        res = bionet_node_get_resource_by_index(node, 24);
        bionet_resource_set_float(res, content, NULL);
    }

    // send command 101. requests analog1's value
    // read value and set resource
    arduino_write(101);
    printf("\nanalog2\n");
    err = arduino_read_until(response, 'x');
    if(err == 0)
    {
        printf("%s\n", response);
        data = atoi(response);
        content = data*ANALOG_INPUT_CONVERSION;
        res = bionet_node_get_resource_by_index(node, 25);
        bionet_resource_set_float(res, content, NULL);
    }

    // send command 200. requests the 8 digital values
    // read values and set resource
    arduino_write(200);
    printf("\ndigital\n");
    err = arduino_read_until(response, 'x');
    if(err == 0)
    {
        printf("%s\n", response);
        for(int i=0; i<8; i++)
        {
            data = atoi(&response[i]);
            res = bionet_node_get_resource_by_index(node, 16+i);
            bionet_resource_get_binary(res, &oldVal, NULL);
            if(data != oldVal)
                bionet_resource_set_binary(res, data, NULL);
        }   
    }
    // report new data
    hab_report_datapoints(node);

    printf("\n----------------------------------------------\n");
    return 1;
}  
    

    
