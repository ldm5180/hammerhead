
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <limits.h>
#include <syslog.h>

#include <glib.h>

#include "bionet-data-manager.h"
#include "bdm-util.h"
#include "config.h"

GMainLoop *bdm_main_loop = NULL;

#define DB_NAME "bdm.db"

char * database_file = DB_NAME;
extern char bdm_id[256];

GSList * sync_config_list = NULL;
static GSList * sync_thread_list = NULL;
static int bp_attached = 0;

int bdm_shutdown_now = 0;
#ifdef ENABLE_ION
client_t dtn_thread_data = {0};
#endif

void usage(void) {
    printf(
	"'bionet-data-manager' records Bionet traffic to a database.\n"
	"\n"
	"usage: bionet-data-manager [OPTIONS]\n"
	"\n"
	" -?,--help                                      Show this help\n"
	" -c,--sync-sender-config <FILE>                 File for configuring a BDM sync sender\n"
#if ENABLE_ION
	" -d,--dtn-sync-receiver                         Enable BDM syncronization over DTN (ION)\n"
#endif
	" -e,--require-security                          Require security\n"
	" -f,--file /Path/to/database/file.db            Full path of database file (bdm.db)\n"
	" -h,--hab,--habs \"HAB-Type.Hab-ID\"              Subscribe to a HAB list.\n"
	" -i,--id <ID>                                   ID of Bionet Data Manager\n"
#if ENABLE_ION
#if HAVE_SM_SET_BASEKEY
	" --ion-key <int>                                Alternate ION key to use if syncing over ION\n"       
#endif
#endif
	" -n,--node,--nodes \"HAB-Type.HAB-ID.Node-ID\"    Subscribe to a Node list.\n"
#if ENABLE_ION
	" -o,--dtn-endpoint-id <ID>                      DTN endpoint ID (ex: ipn:1.2)\n"
#endif
        " -p,--port <port>                               Alternate BDM Client port. Default: %u\n"
	" -r,--resource,--resources \"HAB-Type.HAB-ID.Node-ID:Resource-ID\"\n"
	"                                                Subscribe to Resource values.\n"
	" -s,--security-dir <dir>                        Directory containing security\n"
	"                                                certificates\n"
	" -t,--tcp-sync-receiver [<port>]                Enable BDM synchonization over TCP. \n"
        "                                                Optionally specify the tcp port. Default: %d\n"
	" -v,--version                                   Show the version number\n"
	"\n"
	"Security can only be required when a security directory has been specified.\n"
	"  bionet-data-manager [--security-dir <dir> [--require-security]]\n",
        BDM_PORT,
        BDM_SYNC_PORT);
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

    int tcp_sync_recv_port = BDM_SYNC_PORT;
    int bdm_port = BDM_PORT;
    int enable_tcp_sync_receiver = 0;
    int enable_dtn_sync_receiver = 0;

    //
    // parse command-line arguments
    //
    int i = 0;
    int c;
    while(1) {
	static struct option long_options[] = {
	    {"help",               0, 0, '?'},
	    {"version",            0, 0, 'v'},
	    {"file",               1, 0, 'f'},
	    {"habs",               1, 0, 'h'},
	    {"hab",                1, 0, 'h'},
	    {"ion-key",            1, 0, 'I'},
	    {"id",                 1, 0, 'i'},
	    {"nodes",              1, 0, 'n'},
	    {"node",               1, 0, 'n'},
	    {"resources",          1, 0, 'r'},
	    {"resource",           1, 0, 'r'},
	    {"require-security",   0, 0, 'e'},
	    {"security-dir",       1, 0, 's'},
	    {"tcp-sync-receiver",  2, 0, 't'},
	    {"sync-sender-config", 1, 0, 'c'},
	    {"port",               1, 0, 'p'},
	    {"dtn-sync-receiver",  0, 0, 'd'},
	    {"dtn-endpoint-id",    1, 0, 'o'},
	    {0, 0, 0, 0} //this must be last in the list
	};

	c= getopt_long(argc, argv, "?vedt::f:h:I:i:n:p:r:s:c:o:", long_options, &i);
	if ((-1) == c) {
	    break;
	}

	switch (c) {

	case '?':
	    usage();
	    return 0;

	case 'c':
	{
	    sync_sender_config_t * sync_config = NULL;
	    sync_config = read_config_file(optarg);
	    if (NULL == sync_config) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		      "Failed to parse config file %s", optarg);
		continue;
	    }
	    sync_config->last_entry_end_seq = -1;
	    sync_config->last_entry_end_seq_metadata = -1;
            if(sync_config->method == BDM_SYNC_METHOD_ION && !bp_attached){
#if ENABLE_ION
                if (bp_attach() < 0)
                {
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                        "Can't attach to BP.");
                    continue;
                }
                bp_attached++;
#else
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
                      "Bad config file '%s': BDM Syncronization over DTN was disabled at compile time.", optarg);
                return (1);
#endif
            }
	    sync_config_list = g_slist_append(sync_config_list, sync_config);
	    break;
	}

	case 'd':
#if ENABLE_ION
	    enable_dtn_sync_receiver = 1;
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
		  "Starting BDM-DTN sync receiver");
	    //BDM-BP TODO: complete me!
#else
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
		  "BDM Syncronization over DTN was disabled at compile time.");
	    return (1);
#endif
	    break;

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
	    strncpy(bdm_id, optarg, sizeof(bdm_id) - 1);
	    break;

	case 'I':
        {
#if HAVE_SM_SET_BASEKEY
            char * endptr = NULL;
            long key;
            key = strtoul(optarg, &endptr, 10);
            if(endptr > optarg 
            && endptr[0] == '\0')
            {
                sm_set_basekey(key);
            } else {
                g_warning("invalid ION key specified: '%s'", optarg); 
            }
#else	
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
		  "Setting ION basekey not supported");
	    return (1);
#endif // HAVE_SM_SET_BASEKEY
	    break;
        }

	case 'n':
	    if (node_list_index < MAX_SUBSCRIPTIONS) {
		node_list_name_patterns[node_list_index] = optarg;
		node_list_index++;
	    } else {
		g_warning("skipping Node subscription %s, only %d are handled", 
			  *argv, MAX_SUBSCRIPTIONS);
	    }
	    break;

	case 'o':
#if ENABLE_ION
	    dtn_endpoint_id = optarg;
#else
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
		  "DTN capabilities were disabled at compile time.");
	    return (-1);
#endif
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
        {
	    enable_tcp_sync_receiver = 1;
            if(optarg && optarg[0] != '\0') {
                char * endptr = NULL;
                long tcp_port;
                tcp_port = strtoul(optarg, &endptr, 10);
                if(endptr > optarg 
                && endptr[0] == '\0' 
                && tcp_port < USHRT_MAX)
                {
                    tcp_sync_recv_port = tcp_port;
                } else {
                    g_warning("invalid tcp sync-recieve port specified: '%s'", optarg); 
                }
            }
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
		  "Starting BDM-TCP sync receiver on port %d", tcp_sync_recv_port);
	    break;
        }

	case 'p':
        {
            char * endptr = NULL;
            long tcp_port;
            tcp_port = strtoul(optarg, &endptr, 10);
            if(endptr > optarg 
            && endptr[0] == '\0' 
            && tcp_port < USHRT_MAX)
            {
                bdm_port = tcp_port;
            } else {
                g_warning("invalid tcp port specified: '%s'", optarg); 
            }
	    break;
        }

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

    if ((main_db = db_init()) == NULL) {
        // an informative error message has already been logged by db_init()
        exit(1);
    }

    // Add self as bdm to db
    db_add_bdm(main_db, bdm_id);


   

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

        fd = make_listening_socket(bdm_port);
        ch = g_io_channel_unix_new(fd);
        g_io_add_watch(ch, G_IO_IN, client_connecting_handler, GINT_TO_POINTER(fd));
    }

    
    // add the TCP sync receiver
    if (enable_tcp_sync_receiver) {
	GIOChannel *ch;
	int fd;
	
	fd = make_listening_socket(tcp_sync_recv_port);
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

    openlog(NULL, LOG_PID, LOG_USER);
    lc.destination = BIONET_LOG_TO_SYSLOG;

    make_shutdowns_clean();



    if (sync_config_list || enable_dtn_sync_receiver) {
	g_thread_init(NULL);
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	      "Initializing GThreads.");
    }

    //create a thread for each sync sender configuration
    for (i = 0; i < g_slist_length(sync_config_list); i++) {
	GThread * thread;
	sync_sender_config_t * sync_config = NULL;

	//init the latest entry end time for the config
	sync_config = g_slist_nth_data(sync_config_list, i);
        sync_config->db = db_init();
	if (sync_config) {
	    sync_config->last_entry_end_seq = 
                db_get_last_sync_seq_datapoints(sync_config->db, 
                    sync_config->sync_recipient);

	    sync_config->last_entry_end_seq_metadata = 
                db_get_last_sync_seq_metadata(sync_config->db, 
                    sync_config->sync_recipient);
	} else {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
		  "Config number %d is not in the list.", i);
	}

	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
	      "Starting sync thread");
	thread = g_thread_create(sync_thread, g_slist_nth_data(sync_config_list, i), TRUE, NULL);
	
	if (thread) {
	    sync_thread_list = g_slist_append(sync_thread_list, thread);
	} else {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "Failed to create a thread for config %d", i);
	}
    }


#if ENABLE_ION
    //start the DTN receiver thread
    GThread * dtn_recv = NULL;
    if (enable_dtn_sync_receiver) {
	if (NULL == dtn_endpoint_id) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
		  "DTN Sync Receiver requested, but no DTN endpoint ID specified.");
	    return (-1);
	}
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
	      "DTN Sync Receiver starting. DTN endpoint ID: %s", dtn_endpoint_id);

        if(!bp_attached){
            if (bp_attach() < 0)
            {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                    "Can't attach to BP.");
                return -1;
            }
            bp_attached++;
        }

	dtn_recv = g_thread_create(dtn_receive_thread, &dtn_thread_data, TRUE, NULL);
	if (NULL == dtn_recv) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "Failed to create DTN receiver thread: %m");
	}
    }
#endif


    //
    //  The program has now initialized itself.
    //  Now go into the main loop.  This selects on the listening socket
    //  and all the clients' sockets, and handles I/O events as they come.
    //

    while (g_main_loop_is_running(bdm_main_loop)) {
        g_main_context_iteration(NULL, TRUE);
    }

    for (i = 0; i < g_slist_length(sync_thread_list); i++) {
        g_thread_join(g_slist_nth_data(sync_thread_list, i));
    }

#if ENABLE_ION
    g_thread_join(dtn_recv);
#endif
    

    // 
    // if we get here, then the main loop has quit and we need to exit
    //

    g_main_loop_unref(bdm_main_loop);
    db_shutdown(main_db);
    return 0;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
