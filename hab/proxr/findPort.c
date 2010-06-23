#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "proxrcmds.h"
#include "proxrport.h"
#include "arduinoport.h"

int main(void)
{
    char arduino_port[] = "/dev/ttyUSB0\0";
    char proxr_port[] = "/dev/ttyUSB0\0";
    char arduino_resp[254];
    char buf[2];
    int len, resp;
    int i = 0;
    
    len = strlen(arduino_port);
    len--;

    for(; i<5; i++)
    {
        sprintf(buf, "%d", i);
        arduino_port[len] = buf[0];
        resp =  arduino_connect(arduino_port);
        if(resp > 0)
        {
            arduino_write(254);
            arduino_read_until(arduino_resp, '\n');
            if(strcmp(arduino_resp, "ok"))
            {
                printf("arduino located at:%s\n", arduino_port);
                break;
            }
        arduino_disconnect();
        }
    }
    
    i = 0;
   
    for(; i<5; i++)
    {
        sprintf(buf, "%d", i);
        proxr_port[len] = buf[0];
        if(strcmp(arduino_port, proxr_port) != 0)
        {
            resp =  proxr_connect(proxr_port);
            if(resp > 0)
            {
                set_reporting_mode(true);
                resp = get_data();
                if(resp == 85)
                {
                    printf("proxr located at:%s\n", proxr_port);
                    set_reporting_mode(false);
                    break;
                }
             proxr_disconnect();
            }
        }
    }
    return 0;
}
