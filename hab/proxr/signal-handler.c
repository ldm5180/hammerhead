#include "sim-hab.h"

//
// signal handler used for SIGINT's and SIGTERM's 
//
void signal_handler(int unused)
{
    printf("\n");
    g_message("Exiting...");
    should_exit = 1;
}
