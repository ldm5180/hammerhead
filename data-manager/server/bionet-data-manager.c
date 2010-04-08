
// Copyright (c) 2008-2010, Regents of the University of Colorado.
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
#include "config.h"
#include "cal-server.h"
#include "hardware-abstractor.h"

GMainLoop *bdm_main_loop = NULL;


#define DB_NAME "bdm.db"

char * database_file = DB_NAME;

void * libbdm_cal_handle = NULL;
bionet_bdm_t * this_bdm = NULL;

GSList * sync_config_list = NULL;
static GSList * sync_thread_list = NULL;
static int bp_attached = 0;

static int hab_fd = -1;
static int start_hab = 0;
static unsigned int bdm_stats = 300;

int bdm_shutdown_now = 0;
#ifdef ENABLE_ION
client_t dtn_thread_data = {0};
#endif

uint32_t num_sync_datapoints = 0;
uint32_t num_bionet_datapoints = 0;
uint32_t num_this_created = 3;

extern int no_resources;

void usage(void) {
    printf(
	"'bionet-data-manager' records Bionet traffic to a database.\n"
	"\n"
	"usage: bionet-data-manager [OPTIONS]\n"
	"\n"
	" -?,--help                                      Show this help\n"
	" -b,--bdm-stats <SEC>                           Publish BDM statistics to Bionet every SEC seconds.\n"
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
	" -u,--no-resources                              Do not subscribe to any resources. Useful\n"
	"                                                with --tcp-sync-receiver\n"
#if ENABLE_ION
	"                                                and  --dtn-sync-receiver\n"
#endif
	" -v,--version                                   Show the version number\n"
	"\n"
	"Security can only be required when a security directory has been specified.\n"
	"  bionet-data-manager [--security-dir <dir> [--require-security]]\n",
        BDM_PORT,
        BDM_SYNC_PORT);
}


int hab_readable_handler(GIOChannel *listening_ch,
			 GIOCondition condition,
			 gpointer usr_data) {
    hab_read();
    return 1;
} /* hab_readable_handler() */


int cal_readable_handler(
        GIOChannel *listening_ch,
        GIOCondition condition,
        gpointer usr_data) 
{
    struct timeval timeout;


    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    return cal_server.read(libbdm_cal_handle, &timeout);
}


gboolean update_hab(gpointer usr_data) {
    uint32_t num_real = num_bionet_datapoints - num_this_created;
    uint32_t local_last = 0;
    uint32_t dtn_last = 0;
    struct timeval tv;

    if (gettimeofday(&tv, NULL)) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Failed to get time of day.");
    }


    bionet_hab_t * hab = (bionet_hab_t *)usr_data;
    if (NULL == hab) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "NULL HAB passed in.");
	goto ret;
    }

    bionet_node_t * node = bionet_hab_get_node_by_id(hab, "Statistics");
    if (NULL == node) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "Statistics node not found.");
	goto ret;
    }

    bionet_resource_t * dtn = bionet_node_get_resource_by_id(node, "Number-of-Datapoints-over-DTN-Recorded");
    if (NULL == dtn) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "Num DTN Datapoints resource not found");
	return 1;
    } else {
	uint32_t cur;
	struct timeval tv;
	bionet_resource_get_uint32(dtn, &cur, &tv);
	dtn_last = cur;
	if (cur != num_sync_datapoints) {
	    bionet_resource_set_uint32(dtn, num_sync_datapoints, &tv);
	    num_this_created++;
	}
    }

    bionet_resource_t * local = bionet_node_get_resource_by_id(node, "Number-of-Local-Datapoints-Recorded");
    if (NULL == local) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "Num Local Datapoints resource not found");
	return 1;
    } else {
	uint32_t cur;
	struct timeval tv;
	bionet_resource_get_uint32(local, &cur, &tv);
	local_last = cur;
	if (cur != num_real) {
	    bionet_resource_set_uint32(local, num_real, &tv);
	    num_this_created++;
	}
    }
    
    bionet_resource_t * dtn_rate = bionet_node_get_resource_by_id(node, "DTN-Datapoints-Per-Second");
    if (NULL == dtn_rate) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "DTN Datapoints Per Second resource not found");
	return 1;
    } else {
	uint32_t cur;
	if (0 == bdm_stats) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "BDM stats interval is 0. Invalid!");
	    return 1;
	}
	cur = (float)(num_sync_datapoints - dtn_last)/(float)bdm_stats;
	bionet_resource_set_float(dtn_rate, cur, &tv);
	num_this_created++;
    }

    bionet_resource_t * local_rate = bionet_node_get_resource_by_id(node, "Local-Datapoints-Per-Second");
    if (NULL == local_rate) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "Local Datapoints Per Second resource not found");
	return 1;
    } else {
	uint32_t cur;
	if (0 == bdm_stats) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "BDM stats interval is 0. Invalid!");
	    return 1;
	}
	cur = (float)(num_real - local_last)/(float)bdm_stats;
	bionet_resource_set_float(local_rate, cur, &tv);
	num_this_created++;
    }

    hab_report_datapoints(node);

ret:
    return TRUE;
} /* update_hab() */


int main(int argc, char *argv[]) {
    //
    // we'll be using glib, so capture its log messages
    //
    const char * bdm_id = NULL;

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
	    {"bdm-stats",          1, 0, 'b'},
	    {"no-resources",       0, 0, 'u'},
	    {0, 0, 0, 0} //this must be last in the list
	};

	c= getopt_long(argc, argv, "?vedbt::f:h:I:i:n:p:r:s:c:o:", long_options, &i);
	if ((-1) == c) {
	    break;
	}

	switch (c) {

	case '?':
	case ':':
	    usage();
	    return 1;

	case 'b':
	    start_hab = 1;
	    bdm_stats = strtoul(optarg, NULL, 10);
	    break;

	case 'c':
	{
	    sync_sender_config_t * sync_config = NULL;
	    sync_config = read_config_file(optarg);
	    if (NULL == sync_config) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
		      "Failed to parse config file %s", optarg);
                return 1;
	    }
	    sync_config->last_entry_end_seq = -1;
	    sync_config->last_entry_end_seq_metadata = -1;
            if(sync_config->method == BDM_SYNC_METHOD_ION && !bp_attached){
#if ENABLE_ION
                if (bp_attach() < 0)
                {
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
                        "Can't attach to BP, but DTN syncing requested");
                    return 1
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
            bdm_id = optarg;
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
            printf("processing --resource %s\n", optarg);
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
            tcp_port = strtol(optarg, &endptr, 10);
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

	case 'u':
	    no_resources = 1;
	    break;

	case 'v':
	    print_bionet_version(stdout);
	    return 0;
	    
	default:
            printf("Unknown option\n");
	    usage();
            return 1;
	    break;
	}
    } //while(1)

    if(optind < argc){
        printf("Extra options\n");
        usage();
        return 1;
    }


    this_bdm = bionet_bdm_new(bdm_id);
    if (NULL == this_bdm ) {
        // Error already logged
        return 1;
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
    db_add_bdm(main_db, bionet_bdm_get_id(this_bdm));


   

    //
    // create the main loop, using the default context, and mark it as "running"
    //

    bdm_main_loop = g_main_loop_new(NULL, TRUE);


    // start the BDM HAB
    bionet_hab_t * hab = NULL;
    if (start_hab) {
	
	hab = bionet_hab_new("Bionet-Data-Manager", bdm_id);
	if (NULL == hab) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize Statistics HAB.");
	    exit(1);
	} else {
	    hab_fd = hab_connect(hab);
	    if (hab_fd == -1) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to connect HAB to Bionet");
		exit(1);
	    }
	    hab_read();
	}

	bionet_node_t * node = bionet_node_new(hab, "Statistics");
	if (NULL == node) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize Statistics node.");
	    exit(1);
	}


	/* Time Started */
	bionet_resource_t * resource = bionet_resource_new(node, 
							   BIONET_RESOURCE_DATA_TYPE_UINT32,
							   BIONET_RESOURCE_FLAVOR_SENSOR,
							   "Time-Started");
	if (NULL == resource) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize Time Started resource.");
	    exit(1);
	}

	if (bionet_node_add_resource(node, resource)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
		  "Failed to add Time Started resource to Statistics node");
	    exit(1);
	}

	struct timeval tv;
	if (gettimeofday(&tv, NULL)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Failed to get time of day: %m");
	    exit(1);
	} else {
	    if (bionet_resource_set_uint32(resource, (uint32_t)tv.tv_sec, &tv)) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Failed to set Time Started resource value");
	    }
	}


	/* DTN Datapoints */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT32,
				       BIONET_RESOURCE_FLAVOR_SENSOR,
				       "Number-of-Datapoints-over-DTN-Recorded");
	if (NULL == resource) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize Num DTN Datapoints resource.");
	    exit(1);
	}

	if (bionet_node_add_resource(node, resource)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "Failed to add Num DTN Datapoints resource to Statistics node");
	    exit(1);
	}

	if (bionet_resource_set_uint32(resource, num_sync_datapoints, &tv)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Failed to set Num DTN Datapoints resource value");
	}


	/* Bionet Datapoints */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT32,
				       BIONET_RESOURCE_FLAVOR_SENSOR,
				       "Number-of-Local-Datapoints-Recorded");
	if (NULL == resource) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize Num Local Datapoints resource.");
	    exit(1);
	}

	if (bionet_node_add_resource(node, resource)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "Failed to add Num Local Datapoints resource to Statistics node");
	    exit(1);
	}

	if (bionet_resource_set_uint32(resource, num_bionet_datapoints, &tv)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Failed to set Num Local Datapoints resource value");
	}

	/* DTN Datapoints Rate */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_FLOAT,
				       BIONET_RESOURCE_FLAVOR_SENSOR,
				       "DTN-Datapoints-Per-Second");
	if (NULL == resource) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize DTN Datapoints Per Second resource.");
	    exit(1);
	}

	if (bionet_node_add_resource(node, resource)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "Failed to add DTN Datapoints Per Second resource to Statistics node");
	    exit(1);
	}

	if (0 == bdm_stats) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "BDM stats interval is 0. Invalid!");
	    exit(1);
	}
	if (bionet_resource_set_float(resource, (float)num_sync_datapoints/(float)bdm_stats, &tv)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Failed to set DTN Datapoints Per Second resource value");
	}

	/* Bionet Datapoints Rate */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_FLOAT,
				       BIONET_RESOURCE_FLAVOR_SENSOR,
				       "Local-Datapoints-Per-Second");
	if (NULL == resource) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize Local Datapoints Per Second resource.");
	    exit(1);
	}

	if (bionet_node_add_resource(node, resource)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "Failed to add Local Datapoints Per Second resource to Statistics node");
	    exit(1);
	}

	if (0 == bdm_stats) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "BDM stats interval is 0. Invalid!");
	    exit(1);
	}
	if (bionet_resource_set_float(resource, (float)num_bionet_datapoints/(float)bdm_stats, &tv)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Failed to set Local Datapoints Per Second resource value");
	}


	/* Add node */
	if (bionet_hab_add_node(hab, node)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
		  "Failed to add Statistics node to BDM HAB");
	    exit(1);
	}

	/* Report node and datapoints */
	hab_report_new_node(node);
	hab_report_datapoints(node);

	// watch the hab fd 
        GIOChannel *ch;

        ch = g_io_channel_unix_new(hab_fd);
        g_io_add_watch(ch, G_IO_IN, hab_readable_handler, GINT_TO_POINTER(hab_fd));

	g_timeout_add(bdm_stats * 1000, update_hab, hab);
    }

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



    //
    // Publish BDM service...
    //
    {
        GIOChannel *ch;

        libbdm_cal_handle = 
            cal_server.init("bionet-db", bionet_bdm_get_id(this_bdm), 
			    libbdm_cal_callback, libbdm_cal_topic_matches, NULL, 0);
        if (libbdm_cal_handle == NULL) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error initializing CAL");
            return 1;
        }

	int libbdm_cal_fd = cal_server.get_fd(libbdm_cal_handle);
        ch = g_io_channel_unix_new(libbdm_cal_fd);
        g_io_add_watch(ch, G_IO_IN, cal_readable_handler, NULL);
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




    if (sync_config_list || enable_dtn_sync_receiver) {
        make_shutdowns_clean(1);
    } else {
        make_shutdowns_clean(0);
    }

    //create a thread for each sync sender configuration
    for (i = 0; i < g_slist_length(sync_config_list); i++) {
	GThread * thread;
	sync_sender_config_t * sync_config = NULL;

	//init the latest entry end time for the config
	sync_config = g_slist_nth_data(sync_config_list, i);
	if (sync_config) {
	    sync_config->db = db_init();
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
    if(dtn_recv){
        g_thread_join(dtn_recv);
    }
#endif
    

    // 
    // if we get here, then the main loop has quit and we need to exit
    //

    g_main_loop_unref(bdm_main_loop);
    db_shutdown(main_db);

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
        "Bionet Data Manager shut down cleanly");
    return 0;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
