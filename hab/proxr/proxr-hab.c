#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>

#include "glib.h"

#include "sim-hab.h"
#include "proxrcmds.h"
#include "proxrport.h"

#define HAB_TYPE "sim-hab"

int should_exit = 0;
bionet_hab_t *hab;

int main(int argc, char* argv[])
{
    int bionet_fd;
    int proxr_fd;
    int i;

    char *hab_type = HAB_TYPE;
    char *hab_id = NULL;

    while(1)
    {
        int c;
        static struct option long_options[] =
        {
            {"help", 0, 0, '?'},
            {"version", 0, 0, 'v'},
            {"id", 1, 0, 'i'},
            {0, 0, 0, 0}
        };

        c = getopt_long(argc, argv, "?hvi:", long_options, &i);
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

            default:
                break;
        }
    }

    hab = bionet_hab_new(hab_type, hab_id);

    // register callbacks
    hab_register_callback_set_resource(cb_set_resource);

    // connect to bionet
    bionet_fd = hab_connect(hab);
    if(bionet_fd < 0)
    {
        fprintf(stderr, "Error connecting to Bionet, exiting.\n");
        return 1;
    }

    g_message("%s connected to Bionet!", bionet_hab_get_name(hab));

    // connect to proxr controller
    proxr_fd = proxr_find_and_connect();
    if(proxr_fd < 0)
    {
        g_warning("could not connect to proxr device, exiting");
        return 1;
    }

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // add node
    add_node(hab, "sim1");
    set_all_potentiometers(0);

    //
    // main loop
    //
    while(1)
    {
        hab_read_with_timeout(NULL);

        if(should_exit)
        {
            hab_disconnect();
            proxr_disconnect();
            break;
        }
    }

    return 0;
}
       


