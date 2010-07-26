#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>

#include <glib.h>

#include "sim-hab.h"

#define HAB_TYPE "proxr"

bionet_hab_t *hab;

int main(int argc, char* argv[])
{
    int bionet_fd;
    int proxr_fd;
    int i;

    char *hab_type = HAB_TYPE;
    char *hab_id = NULL;
    char *proxr_loc = NULL;

//    GMainLoop *main_loop;

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);


    while(1)
    {
        int c;
        static struct option long_options[] =
        {
            {"help", 0, 0, '?'},
            {"version", 0, 0, 'v'},
            {"id", 1, 0, 'i'},
            {"proxr", 1, 0, 'p'},
            {0, 0, 0, 0}
        };

        c = getopt_long(argc, argv, "?hvp:i:", long_options, &i);
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

            default:
                break;
        }
    }

    if(proxr_loc == NULL)
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

    // add node
    add_node(hab, "potentiometers");

    // initialize all the pot's to zero
    set_all_potentiometers(0);

    while(1)
    {
	fd_set fds;
	int n;
	struct timeval timeout;

	timeout.tv_sec = 10;
	timeout.tv_usec = 0;

	FD_ZERO(&fds);
	FD_SET(bionet_fd, &fds);
	
	n = select(bionet_fd+1, &fds, NULL, NULL, &timeout);

	if(n < 0)
	    printf("select failed\n");
	else if(n == 0)
	   //timeout printf("TIMEOUT\n");
	else
	{
	    if(FD_ISSET(bionet_fd, &fds))
	    {
		hab_read_with_timeout(NULL);
	    }
	}
	
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
       


