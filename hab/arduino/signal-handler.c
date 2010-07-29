#include "sim-hab.h"
#include <stdlib.h>

//
// signal handler used for SIGINT's and SIGTERM's 
//
void signal_handler(int unused)
{
    arduino_write((char)1);
    hab_disconnect();
    arduino_disconnect();
    printf("\n");
    g_message("Exiting...");
    exit(1);
}
