
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "bionet-data-manager.h"
#include "bdm-util.h"




GMainLoop *bdm_main_loop = NULL;

#define DB_NAME "bdm.db"

char * database_file = DB_NAME;


void usage(void) {
    printf("usage: bionet-data-manager OPTIONS...\n\
\n\
OPTIONS:\n\
\n\
    --habs HAB-Type.Hab-ID\n\
        Subscribe to a HAB list.\n\
\n\
    --nodes HAB-Type.HAB-ID.Node-ID\n\
        Subscribe to a Node list.\n\
\n\
    -r, --resource, --resources HAB-Type.HAB-ID.Node-ID:Resource-ID\n\
        Subscribe to Resource values.\n\
\n\
    -f, --file /Path/to/database/file.db\n\
        Full path of database file (default: bdm.db)\n\
");
}




int main(int argc, char *argv[]) {
    //
    // we'll be using glib, so capture its log messages
    //

    bionet_log_context_t lc = {
        destination: BIONET_LOG_TO_STDOUT,
        log_limit: G_LOG_LEVEL_INFO
    };

    g_log_set_default_handler(bionet_glib_log_handler, &lc);


    //
    // parse command-line arguments
    //

    {
        argv ++;

        for ( ; *argv != NULL; argv ++) {
            if (strcmp(*argv, "--habs") == 0) {
                argv ++;
                if (hab_list_index < MAX_SUBSCRIPTIONS) {
                    hab_list_name_patterns[hab_list_index] = *argv;
                    hab_list_index ++;
                } else {
                    g_warning("skipping HAB subscription %s, only %d are handled", *argv, MAX_SUBSCRIPTIONS);
                    argv ++;
                }

            } else if (strcmp(*argv, "--nodes") == 0) {
                argv ++;
                if (node_list_index < MAX_SUBSCRIPTIONS) {
                    node_list_name_patterns[node_list_index] = *argv;
                    node_list_index ++;
                } else {
                    g_warning("skipping Node subscription %s, only %d are handled", *argv, MAX_SUBSCRIPTIONS);
                    argv ++;
                }

            } else if ((strcmp(*argv, "-r") == 0) || (strcmp(*argv, "--resource") == 0) || (strcmp(*argv, "--resources") == 0)) {
                argv ++;
                if (resource_index < MAX_SUBSCRIPTIONS) {
                    resource_name_patterns[resource_index] = *argv;
                    resource_index ++;
                } else {
                    g_warning("skipping Resource subscription %s, only %d are handled", *argv, MAX_SUBSCRIPTIONS);
                    argv ++;
                }

            } else if ((strcmp(*argv, "-f") == 0) 
		       || (strcmp(*argv, "--file") == 0)) {
		argv++;
		database_file = *argv;
	    } else {
                usage();
                exit(1);
            }
        }
    }




    if (db_init() != 0) {
        // an informative error message has already been logged by db_init()
        exit(1);
    }




    //
    // create the main loop, using the default context, and mark it as "running"
    //

    bdm_main_loop = g_main_loop_new(NULL, TRUE);




    // 
    // create the listening socket for bdm clients
    //
    // This will succeed or exit.
    //

    {
        GIOChannel *ch;
        int fd;

        fd = make_listening_socket(BDM_PORT);
        ch = g_io_channel_unix_new(fd);
        g_io_add_watch(ch, G_IO_IN, client_connecting_handler, GINT_TO_POINTER(fd));
    }




    // 
    // we're not connected to the Bionet, so add an idle function to the main loop to try to connect
    // if this function succeeds in connecting, it'll remove itself from the main loop and add the bionet fd
    // if this function fails, it'll remove itself from the main loop and install a 5-second timeout to retry
    //

    g_idle_add(try_to_connect_to_bionet, NULL);




    //
    //  Here's where we (FIXME) should background ourselves.  This is a
    //  good place because everything that could go wrong, hasn't.
    //

    lc.destination = BIONET_LOG_TO_SYSLOG;

    make_shutdowns_clean();




    //
    //  The program has now initialized itself.
    //  Now go into the main loop.  This selects on the listening socket
    //  and all the clients' sockets, and handles I/O events as they come.
    //

    while (g_main_loop_is_running(bdm_main_loop)) {
        g_main_context_iteration(NULL, TRUE);
    }


    // 
    // if we get here, then the main loop has quit and we need to exit
    //

    g_main_loop_unref(bdm_main_loop);
    db_shutdown();
    return 0;
}

