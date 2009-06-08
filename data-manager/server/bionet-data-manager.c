
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include <glib.h>

#include "bionet-data-manager.h"
#include "bdm-util.h"




GMainLoop *bdm_main_loop = NULL;

#define DB_NAME "bdm.db"

char * database_file = DB_NAME;
extern char bdm_id[256];
int enable_tcp_sync_receiver = 0;

void usage(void) {
    printf(
	"'bionet-data-manager' records Bionet traffic to a database.\n"
	"\n"
	"usage: bionet-data-manager [OPTIONS]\n"
	"\n"
	" -?,--help                                      Show this help\n"
	" -e,--require-security                          Require security\n"
	" -f,--file /Path/to/database/file.db            Full path of database file (bdm.db)\n"
	" -h,--hab,--habs \"HAB-Type.Hab-ID\"              Subscribe to a HAB list.\n"
	" -i,--id <ID>                                   ID of Bionet Data Manager\n"
	" -n,--node,--nodes \"HAB-Type.HAB-ID.Node-ID\"    Subscribe to a Node list.\n"
	" -r,--resource,--resources \"HAB-Type.HAB-ID.Node-ID:Resource-ID\"\n"
	"                                                Subscribe to Resource values.\n"
	" -s,--security-dir <dir>                        Directory containing security\n"
	"                                                certificates\n"
	" -t,--tcp-sync-receiver                         Enable BDM synchonization over TCP\n"
	" -v,--version                                   Show the version number\n"
	"\n"
	"Security can only be required when a security directory has been specified.\n"
	"  bionet-data-manager [--security-dir <dir> [--require-security]]\n");
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
    int i = 0;
    int c;
    while(1) {
	static struct option long_options[] = {
	    {"help", 0, 0, '?'},
	    {"version", 0, 0, 'v'},
	    {"file", 1, 0, 'f'},
	    {"habs", 1, 0, 'h'},
	    {"hab", 1, 0, 'h'},
	    {"id", 1, 0, 'i'},
	    {"nodes", 1, 0, 'n'},
	    {"node", 1, 0, 'n'},
	    {"resources", 1, 0, 'r'},
	    {"resource", 1, 0, 'r'},
	    {"require-security", 0, 0, 'e'},
	    {"security-dir", 1, 0, 's'},
	    {"tcp-sync-receiver", 0, 0, 't'},
	    {0, 0, 0, 0} //this must be last in the list
	};

	c= getopt_long(argc, argv, "?vetf:h:i:n:r:s:", long_options, &i);
	if ((-1) == c) {
	    break;
	}

	switch (c) {

	case '?':
	    usage();
	    return 0;

	case 'e':
	    if (security_dir) {
		require_security++;
	    } else {
		usage();
		return (-1);
	    }
	    break;

	case 'f':
	    database_file = optarg;
	    break;

	case 'h':
	    if (hab_list_index < MAX_SUBSCRIPTIONS) {
		hab_list_name_patterns[hab_list_index] = optarg;
		hab_list_index++;
	    } else {
		g_warning("skipping HAB subscription %s, only %d are handled", 
			  *argv, MAX_SUBSCRIPTIONS);
	    }
	    break;

	case 'i':
	    strncpy(bdm_id, optarg, sizeof(bdm_id));
	    break;

	case 'n':
	    if (node_list_index < MAX_SUBSCRIPTIONS) {
		node_list_name_patterns[node_list_index] = optarg;
		node_list_index++;
	    } else {
		g_warning("skipping Node subscription %s, only %d are handled", 
			  *argv, MAX_SUBSCRIPTIONS);
	    }
	    break;

	case 'r':
	    if (resource_index < MAX_SUBSCRIPTIONS) {
		resource_name_patterns[resource_index] = optarg;
		resource_index++;
	    } else {
		g_warning("skipping Resource subscription %s, only %d are handled", 
			  *argv, MAX_SUBSCRIPTIONS);
	    }
	    break;

	case 's':
	    security_dir = optarg;
	    break;

	case 't':
	    enable_tcp_sync_receiver = 1;
	    break;

	case 'v':
	    print_bionet_version(stdout);
	    return 0;
	    
	default:
	    break;
	}
    } //while(1)


    if (0 == bdm_id[0]) {
	int r;
	r = gethostname(bdm_id, sizeof(bdm_id));
	if (r < 0) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Error getting hostname: %m");
	    exit(1);
	}
    }

    if (security_dir) {
	if (bionet_init_security(security_dir, require_security)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Failed to initialize security.");
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

    
    // add the TCP sync receiver
    if (enable_tcp_sync_receiver) {
	GIOChannel *ch;
	int fd;
	
	fd = make_listening_socket(BDM_SYNC_PORT);
	ch = g_io_channel_unix_new(fd);
	g_io_add_watch(ch, G_IO_IN, sync_receive_connecting_handler, GINT_TO_POINTER(fd));
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

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
