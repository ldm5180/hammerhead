#include "sim-hab.h"
#include <stdlib.h>

//
// signal handler used for SIGINT's and SIGTERM's 
//
void signal_handler(int unused)
{
    printf("\n");
    g_message("Exiting...");
    hab_disconnect();
    proxr_disconnect();
    arduino_disconnect();
    exit(1);
}
