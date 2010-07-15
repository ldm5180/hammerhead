#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>

#include <glib.h>

#include "sim-hab.h"

#define HAB_TYPE "cgba-sim"

bionet_hab_t *hab;

int main(int argc, char* argv[])
{
    char a = '\n';
    int bionet_fd;
    int proxr_fd;
    int arduino_fd;
    int i;

    char *hab_type = HAB_TYPE;
    char *hab_id = NULL;
    char *proxr_loc = NULL;
    char *arduino_loc = NULL;

//    GMainLoop *main_loop;

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
            {"proxr", 1, 0, 'p'},
            {"arduino", 1, 0, 'a'},
            {0, 0, 0, 0}
        };

        c = getopt_long(argc, argv, "?hva:p:i:", long_options, &i);
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

            case 'p':
                proxr_loc = optarg;
                break;

            case 'a':
                arduino_loc = optarg;
                break;

            default:
                break;
        }
    }

    if(proxr_loc == NULL || arduino_loc == NULL)
    {
        usage();
        return 0;
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
    proxr_fd = proxr_connect(proxr_loc);
    if(proxr_fd < 0)
    {
        g_warning("could not connect to proxr device, exiting");
        return 1;
    }

    arduino_fd = arduino_connect(arduino_loc);
    if(arduino_fd < 0)
    {
        g_warning("could not connect to arduino device, exiting");
        return 1;
    }

    // add node
    add_node(hab, "sim");
    
    set_all_potentiometers(0);
    arduino_write(1);

    while(1)
    {
        poll_arduino(bionet_fd);
    }

   /* main_loop = g_main_loop_new(NULL, TRUE);

    {
        GIOChannel *ch;
        ch = g_io_channel_unix_new(bionet_fd);
        g_io_add_watch(ch, G_IO_IN, read_from_bionet, NULL);
    }

    g_timeout_add(2000, poll_arduino, NULL);
    
    g_main_loop_run(main_loop);
    */
    return 0;
}
       


