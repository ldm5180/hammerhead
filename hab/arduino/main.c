#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>

#include <glib.h>

#include "sim-hab.h"

#define HAB_TYPE "arduino"

bionet_hab_t *hab;

int main(int argc, char* argv[])
{
    int bionet_fd;
    int arduino_fd;
    int i;

    char *hab_type = HAB_TYPE;
    char *hab_id = NULL;
    char *arduino_loc = NULL;

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGIO, SIG_IGN);


    while(1)
    {
        int c;
        static struct option long_options[] =
        {
            {"help", 0, 0, '?'},
            {"version", 0, 0, 'v'},
            {"id", 1, 0, 'i'},
            {"arduino", 1, 0, 'a'},
            {0, 0, 0, 0}
        };

        c = getopt_long(argc, argv, "?hva:i:", long_options, &i);
        if(c == -1)
        {
            break;
        }

        switch (c)
        {
            case '?':
            case 'h':
                usage();
                return 0;

            case 'i':
                hab_id = optarg;
                break;

            case 'v':
                print_bionet_version(stdout);
                return 0;

            case 'a':
                arduino_loc = optarg;
                break;

            default:
                break;
        }
    }

    if(arduino_loc == NULL)
    {
        usage();
        return 0;
    }

    hab = bionet_hab_new(hab_type, hab_id);

    // register callbacks ... none

    // connect to bionet
    bionet_fd = hab_connect(hab);
    if(bionet_fd < 0)
    {
        fprintf(stderr, "Error connecting to Bionet, exiting.\n");
        return 1;
    }

    g_message("%s connected to Bionet!", bionet_hab_get_name(hab));

    arduino_fd = arduino_connect(arduino_loc);
    if(arduino_fd < 0)
    {
        g_warning("could not connect to arduino device, exiting");
        return 1;
    }

    // add node
    add_node(hab, "resources");
    
    //tell arduino board to activate
    arduino_write((char)1);
    while(1)
    {
        poll_arduino(bionet_fd);
    }

    return 0;
}
       


