#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "proxrcmds.h"
#include "proxrport.h"
#include "arduinoport.h"

int main(void)
{
    char port[] = "/dev/ttyUSB0\0";
    char arduino_resp[254];
    char buf[2];
    int len, resp;
    int i = 0;
    
    len = strlen(port);
    len--;

    for(; i<5; i++)
    {
        sprintf(buf, "%d", i);
        port[len] = buf[0];
        resp =  arduino_connect(port);
        if(resp > 0)
        {
            arduino_write(254);
            arduino_read_until(arduino_resp, '\n');
            if(strcmp(arduino_resp, "ok"))
            {
                printf("arduino located at:%s\n", port);
                break;
            }
        arduino_disconnect();
        }
    }
    
    i = 0;
   
    for(; i<5; i++)
    {
        sprintf(buf, "%d", i);
        port[len] = buf[0];
        resp =  proxr_connect(port);
        if(resp > 0)
        {
            set_reporting_mode(true);
            resp = get_data();
            if(resp == 85)
            {
                printf("proxr located at:%s\n", port);
                set_reporting_mode(false);
                break;
            }
        proxr_disconnect();
        }
    }
    return 0;
}
