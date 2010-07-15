#include "sim-hab.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int poll_arduino(int bionet_fd)
{
    char response[256];
    char b[1];

    bionet_resource_t *res = NULL;
    bionet_node_t *node = NULL;

    int n, data;
    float content;
    int arduino_fd = get_arduino_fd();
    int max_fd;
    fd_set fds;
    struct timeval timeout;

    node = bionet_hab_get_node_by_index(hab, 0);

    FD_ZERO(&fds);
    FD_SET(arduino_fd, &fds);
    FD_SET(bionet_fd, &fds);

    max_fd = (arduino_fd > bionet_fd ? arduino_fd : bionet_fd);

    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    n = select(max_fd+1, &fds, NULL, NULL, &timeout);

    if(n < 0)
        printf("select failed\n");
    else if(n == 0)
        printf("TIMEOUT\n");
    else
    {
        
        if(FD_ISSET(arduino_fd, &fds))
        {
    
            read(arduino_fd, b, 1);

            switch((int)b[0])
            {
                case 100: // ANALOG INPUT 0
                    arduino_read_until(response, '\n');
                    data = atoi(response);
                    content = data*ANALOG_INPUT_CONVERSION;
                    res = bionet_node_get_resource_by_index(node, ANALOG_INPUT_0);
                    bionet_resource_set_float(res, content, NULL);
                    break;

                case 101: // ANALOG INPUT 1
                    arduino_read_until(response, '\n');
                    data = atoi(response);
                    content = data*ANALOG_INPUT_CONVERSION;
                    res = bionet_node_get_resource_by_index(node, ANALOG_INPUT_1);
                    bionet_resource_set_float(res, content, NULL);
                    break;

               case 110: // DIGITAL INPUT 0
                    arduino_read_until(response, '\n');
                    data = atoi(response);
                    res = bionet_node_get_resource_by_index(node, DIGITAL_INPUT_0);
                    bionet_resource_set_binary(res, data, NULL);
                    break;
                    
                case 111: // DIGITAL INPUT 1
                    arduino_read_until(response, '\n');
                    data = atoi(response);
                    res = bionet_node_get_resource_by_index(node, DIGITAL_INPUT_1);
                    bionet_resource_set_binary(res, data, NULL);
                    break;

                case 112: // DIGITAL INPUT 2
                    arduino_read_until(response, '\n');
                    data = atoi(response);
                    res = bionet_node_get_resource_by_index(node, DIGITAL_INPUT_2);
                    bionet_resource_set_binary(res, data, NULL);
                    break;

                case 113: // DIGITAL INPUT 3
                    arduino_read_until(response, '\n');
                    data = atoi(response);
                    res = bionet_node_get_resource_by_index(node, DIGITAL_INPUT_3);
                    bionet_resource_set_binary(res, data, NULL);
                    break;

                case 114: // DIGITAL INPUT 4
                    arduino_read_until(response, '\n');
                    data = atoi(response);
                    res = bionet_node_get_resource_by_index(node, DIGITAL_INPUT_4);
                    bionet_resource_set_binary(res, data, NULL);
                    break;

                case 115: // DIGITAL INPUT 5
                    arduino_read_until(response, '\n');
                    data = atoi(response);
                    res = bionet_node_get_resource_by_index(node, DIGITAL_INPUT_5);
                    bionet_resource_set_binary(res, data, NULL);
                    break;

                case 116: // DIGITAL INPUT 6
                    arduino_read_until(response, '\n');
                    data = atoi(response);
                    res = bionet_node_get_resource_by_index(node, DIGITAL_INPUT_6);
                    bionet_resource_set_binary(res, data, NULL);
                    break;

                case 117: // DIGITAL INPUT 7
                    arduino_read_until(response, '\n');
                    data = atoi(response);
                    res = bionet_node_get_resource_by_index(node, DIGITAL_INPUT_7);
                    bionet_resource_set_binary(res, data, NULL);
                    break;
                    
                default:
                    printf("invalid data\n");
                    break;
            }
        }

        if(FD_ISSET(bionet_fd, &fds))
        {
            hab_read_with_timeout(NULL);
        }
    }

    hab_report_datapoints(node);
    return 1;
}  
    

    
