
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <dlfcn.h>
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
#include "bdm-db.h"
#include "config.h"
#include "cal-server.h"
#include "hardware-abstractor.h"

#include "bdm-stats.h"



static gboolean _add_filter_subscription(const char * resource_pattern) ;



// Default config values
#define DB_NAME "bdm.db"

// Configuration variables
static const char * bdm_id = NULL;
static char * database_file = DB_NAME;
static char * bdm_config_file = "/etc/bdm.ini";

static int enable_debug = 0;
static const char * log_file = NULL;
static int keep_self_stats;

static int tcp_sync_recv_port = BDM_SYNC_PORT;
static int bdm_port = BDM_PORT;
static int enable_tcp_sync_receiver = 0;

static int enable_dtn_sync_receiver = 0;
static int ion_key = 0;

static gchar ** sync_cfg_file_list = NULL;
static gchar ** filter_patterns = NULL;

static gchar ** unparsed_args;
static GSList * sync_thread_list = NULL;

static gboolean _opt_version(const char * optname, const char * optarg, void* data, GError ** error) {
    print_bionet_version(stdout);
    exit(0);
}

static gboolean _opt_tcp_sync_receiver(const char * optname, const char * optarg, void* data, GError ** error) {

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
            return 0;
        }
    }

    return 1;
}

static gchar ** strv_append(gchar ** strv, const gchar * str) {
    int i = 0;
    if(strv) {
        i = g_strv_length(strv);
    }
    if(i < MAX_SUBSCRIPTIONS ) {
        strv = realloc(strv, sizeof(gchar *) * (i + 2));
        if (NULL == strv) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Out of memory, cannot append to string array");
            return NULL;
        }
        strv[i] = (gchar*)str;
        strv[i+1] = NULL;
    } else {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Too many subscriptions requested");
            return NULL;
    }

    return strv;

}


#if ENABLE_ION

bdm_bp_funcs_t bdm_bp_funcs;

#define LIBBP_FILE_NAME "libbp.so.0"

int load_ion(void) {
    void *libbp_handle;
    char *error;
    static int loaded_ion = 0;

    if (loaded_ion) return 0;

    libbp_handle = dlopen("libbp.so.0", RTLD_NOW);
    if (libbp_handle == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "failed to dlopen %s: %s", LIBBP_FILE_NAME, dlerror());
        return -1;
    }

    dlerror();    /* Clear any existing error */

    /* Writing: cosine = (double (*)(double)) dlsym(handle, "cos");
    would seem more natural, but the C99 standard leaves
    casting from "void *" to a function pointer undefined.
    The assignment used below is the POSIX.1-2003 (Technical
    Corrigendum 1) workaround; see the Rationale for the
    POSIX specification of dlsym(). */

    bdm_bp_funcs.sm_set_basekey = (sm_set_basekey_t)dlsym(libbp_handle, "sm_set_basekey");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find sm_set_basekey() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.bp_attach = (bp_attach_t)dlsym(libbp_handle, "bp_attach");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find bp_attach() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.bp_interrupt = (bp_interrupt_t)dlsym(libbp_handle, "bp_interrupt");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find bp_interrupt() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.bp_open = (bp_open_t)dlsym(libbp_handle, "bp_open");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find bp_open() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.bp_close = (bp_close_t)dlsym(libbp_handle, "bp_close");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find bp_close() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.bp_get_sdr = (bp_get_sdr_t)dlsym(libbp_handle, "bp_get_sdr");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find bp_get_sdr() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.bp_send = (bp_send_t)dlsym(libbp_handle, "bp_send");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find bp_send() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.bp_receive = (bp_receive_t)dlsym(libbp_handle, "bp_receive");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find bp_receive() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.bp_add_endpoint = (bp_add_endpoint_t)dlsym(libbp_handle, "bp_add_endpoint");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find bp_add_endpoint() in libbp.so: %s, ignoring error", error);
    }

    bdm_bp_funcs.bp_release_delivery = (bp_release_delivery_t)dlsym(libbp_handle, "bp_release_delivery");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find bp_release_delivery() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.Sdr_malloc = (Sdr_malloc_t)dlsym(libbp_handle, "Sdr_malloc");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find Sdr_malloc() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.sdr_begin_xn = (sdr_begin_xn_t)dlsym(libbp_handle, "sdr_begin_xn");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find sdr_begin_xn() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.sdr_cancel_xn = (sdr_cancel_xn_t)dlsym(libbp_handle, "sdr_cancel_xn");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find sdr_cancel_xn() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.sdr_end_xn = (sdr_end_xn_t)dlsym(libbp_handle, "sdr_end_xn");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find sdr_end_xn() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.Sdr_write = (Sdr_write_t)dlsym(libbp_handle, "Sdr_write");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find Sdr_write() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.zco_create = (zco_create_t)dlsym(libbp_handle, "zco_create");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find zco_create() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.zco_append_extent = (zco_append_extent_t)dlsym(libbp_handle, "zco_append_extent");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find zco_append_extent() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.zco_start_receiving = (zco_start_receiving_t)dlsym(libbp_handle, "zco_start_receiving");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find zco_start_receiving() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.zco_receive_source = (zco_receive_source_t)dlsym(libbp_handle, "zco_receive_source");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find zco_receive_source() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.zco_stop_receiving = (zco_stop_receiving_t)dlsym(libbp_handle, "zco_stop_receiving");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find zco_stop_receiving() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.zco_source_data_length = (zco_source_data_length_t)dlsym(libbp_handle, "zco_source_data_length");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find zco_source_data_length() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }


    bdm_bp_funcs.writeErrMemo = (writeErrMemo_t)dlsym(libbp_handle, "writeErrMemo");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find writeErrMemo() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.writeErrmsgMemos = (writeErrmsgMemos_t)dlsym(libbp_handle, "writeErrmsgMemos");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find writeErrmsgMemos() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    loaded_ion = 1;
    return 0;
}

#endif




/*
typedef struct {
  const gchar *long_name;
  gchar        short_name;
  gint         flags;

  GOptionArg   arg;
  gpointer     arg_data;
  
  const gchar *description;
  const gchar *arg_description;
} GOptionEntry;
*/

#define HELP_WRAP "\n                                    "

static GOptionEntry entries[] = {
    {"version",            'v', 
        G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, _opt_version, 
        "Show the version number", 
        NULL
    },
    {"file",               'f', 
        0, G_OPTION_ARG_FILENAME, &database_file, 
        "Full path of database file (bdm.db)." HELP_WRAP
            "Overrides --bdm-config-file. If the" HELP_WRAP
            "file doesn't exist, it will be created",
        "FILE"
    },
#if ENABLE_ION
#if HAVE_SM_SET_BASEKEY
    {"ion-key",            'I', 
        0, G_OPTION_ARG_INT, &ion_key, 
	"Alternate ION key to use if syncing over ION",
        "INT"
    },
#endif
#endif
    {"id",                 'i', 
        0, G_OPTION_ARG_STRING, &bdm_id, 
	"ID of Bionet Data Manager",
        "ID"
    },
    {"habs",               'h', 
        0, G_OPTION_ARG_STRING_ARRAY, &filter_patterns, 
	"DEPRECATED. Use --filter instead",
        "pattern"
    },
    {"hab",                'h', 
        0, G_OPTION_ARG_STRING_ARRAY, &filter_patterns, 
	"DEPRECATED. Use --filter instead",
        "pattern"
    },
    {"nodes",              'n', 
        0, G_OPTION_ARG_STRING_ARRAY, &filter_patterns,
	"DEPRECATED. Use --filter instead",
        "pattern"
    },
    {"node",               'n', 
        0, G_OPTION_ARG_STRING_ARRAY, &filter_patterns,
	"DEPRECATED. Use --filter instead",
        "pattern"
    },
    {"resources",          'r', 
        0, G_OPTION_ARG_STRING_ARRAY, &filter_patterns,
	"DEPRECATED. Use --filter instead",
        "pattern"
    },
    {"resource",           0, 
        0, G_OPTION_ARG_STRING_ARRAY, &filter_patterns,
	"DEPRECATED. Use --filter instead",
        "pattern"
    },
    {"filter",             'l', 
        0, G_OPTION_ARG_STRING_ARRAY, &filter_patterns,
	"\"HAB-Type.HAB-ID.Node-ID:Resource-ID\"" HELP_WRAP
            "Subscribe to everything matching resource-pattern" HELP_WRAP
            "Overrides filter(s) listed in bdm.ini file.",
        "pattern"
    },
    {"require-security",   'e', 
        0, G_OPTION_ARG_NONE, &require_security, 
        "Require security for all bionet subscriptions",
        NULL
    },
    {"security-dir",       's', 
        0, G_OPTION_ARG_FILENAME, &security_dir, 
	"Directory containing security certificates",
        "DIR"
    },
    {"tcp-sync-receiver",  't', 
        G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_CALLBACK, _opt_tcp_sync_receiver, 
	"Enable BDM synchonization over TCP." HELP_WRAP
            "Optionally specify the tcp port. Default: 11003",
        "PORT"
    },
    {"sync-sender-config", 'c', 
        0, G_OPTION_ARG_STRING_ARRAY, &sync_cfg_file_list, 
	"File for configuring a BDM sync sender",
        "FILE"
    },
    {"port",               'p', 
        0, G_OPTION_ARG_INT, &bdm_port,
        "Alternate BDM Client port. Default: 11002",
        "PORT"
    },
    {"dtn-sync-receiver",  'd', 
        0, G_OPTION_ARG_NONE, &enable_dtn_sync_receiver, 
	"Enable BDM syncronization over DTN (ION)",
        NULL
    },
    {"dtn-endpoint-id",    'o', 
        0, G_OPTION_ARG_STRING, &dtn_endpoint_id, 
	"DTN endpoint ID (ex: ipn:1.2)",
        "EID"
    },
    {"bdm-stats",          'b', 
        0, G_OPTION_ARG_INT, &bdm_stats_interval, 
	"Publish BDM statistics to Bionet every SEC seconds",
        "SEC"
    },
    {"keep-stats",         'k', 
        0, G_OPTION_ARG_NONE, &keep_self_stats, 
	"Also count datapoints generated by this BDM",
        NULL
    },
    {"no-resources",       'u', 
        0, G_OPTION_ARG_NONE, &no_resources, 
	"Do not subscribe to any resources." HELP_WRAP
            "Useful with --tcp-sync-receiver"
#if ENABLE_ION
	" and --dtn-sync-receiver"
#endif
        , NULL
    },
    {"bdm-config-file",    'x', 
        0, G_OPTION_ARG_FILENAME, &bdm_config_file, 
	"BDM configuration file to specify location",
        "FILE"
    },
    {"debug",              0, 
        0, G_OPTION_ARG_NONE, &enable_debug, 
	"Enable debug messages",
        NULL
    },
    {"log-file",           0, 
        0, G_OPTION_ARG_FILENAME, &log_file, 
	"Log message to <path>, instead of syslog." HELP_WRAP
            "Use '-' for stdout",
        "FILE"
    },

    // Special case to catch remaining, unhandled args
    {G_OPTION_REMAINING,           0, 
        0, G_OPTION_ARG_STRING_ARRAY, &unparsed_args, 
	NULL, 
        NULL
    },

    { NULL }
};

// Like GOptionEntry, but for parsing key-value files
//
// It's be nice if this could be combined with GOptionEntry,
// but not for now...
typedef struct {
    char * section; // group_name
    char * key;
    char * name; // Used in messages
    GOptionArg type;
    void * val_dest;
}file_opt_t;

file_opt_t fileopts[] = {
    { "BDM", "DBFILE", "Database File", 
        G_OPTION_ARG_FILENAME, &database_file },

    { "BDM", "FILTER", "Subscription Filter", 
        G_OPTION_ARG_STRING_ARRAY, &filter_patterns },

    { "BDM", "ID", "Bdm ID", 
        G_OPTION_ARG_STRING, &bdm_id },
    
    { "BDM", "BdmStats", "BDM Stats Interval", 
        G_OPTION_ARG_INT, &bdm_stats_interval },
    
    { "BDM", "SyncSenderConfig", "Sync Sender Config", 
        G_OPTION_ARG_STRING_ARRAY, &sync_cfg_file_list },
    
    { "BDM", "DtnSyncReceiver", "Dtn Sync Receiver", 
        G_OPTION_ARG_NONE, &enable_dtn_sync_receiver },
    
    { "BDM", "SecurityDir", "Security Directory", 
        G_OPTION_ARG_FILENAME, &security_dir },
    
    { "BDM", "RequireSecurity", "Require Security", 
        G_OPTION_ARG_NONE, &require_security },
    
    { "BDM", "IonKey", "Ion Key", 
        G_OPTION_ARG_INT, &ion_key },
    
    { "BDM", "KeepStats", "Keep Stats", 
        G_OPTION_ARG_NONE, &keep_self_stats },
    
#if ENABLE_ION
    { "BDM", "DtnEndpointID", "Dtn Endpoint Id", 
        G_OPTION_ARG_STRING, &dtn_endpoint_id },
#endif
    
    { "BDM", "BdmPort", "Bdm Port", 
        G_OPTION_ARG_INT, &bdm_port },
    
    { "BDM", "TcpSyncReceiver", "Tcp Sync Receiver", 
        G_OPTION_ARG_NONE, &enable_tcp_sync_receiver },
    
    { "BDM", "TcpSyncReceiverPort", "Tcp Sync Receiver Port", 
        G_OPTION_ARG_INT, &tcp_sync_recv_port },

    { "BDM", "NoResource", "No Resources",
        G_OPTION_ARG_NONE, &no_resources },

    { 0, 0, 0, 0, 0}
};


int database_file_set = 0;
GMainLoop *bdm_main_loop = NULL;


static GSList * sync_config_list = NULL;

static int hab_fd = -1;




extern int optind;

static const char * help_summary = 
"Records Bionet Network Events to a database, and provides it in a number\n"
"of ways:\n"
" - Clients can subscribe to resource patterns, to obtain recorded data\n"
"   and near-realtime updates\n"
" - bionet-data-manager can also sync some or all of its data base with\n"
"   other bionet-data-managers useing either IP or DTN networks\n"
"\n";

static const char * help_description = 
"NOTE: options should be specified in the bdm.ini file. Command line options\n"
"override them.\n"
"\n"
"Security can only be required when a security directory has been specified.\n";

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

    return cal_server.read(libbdm_cal_handle, &timeout, 0);
}




typedef struct {
    FILE * log_to; // If NULL, log to syslog

    // messages with log_level *below* log_limit are logged, all others are dropped
    // FIXME: really we want a default log_limit and then optional per-domain limits
    GLogLevelFlags log_limit;
} bdm_log_context_t;

void bdm_glib_log_handler(
    const gchar *log_domain,
    GLogLevelFlags log_level,
    const gchar *message,
    gpointer log_context
) {
    static bdm_log_context_t default_log_context;

    bdm_log_context_t *lc;


    if (log_context == NULL) {
        lc = &default_log_context;
        lc->log_to = stdout;
        lc->log_limit = G_LOG_LEVEL_INFO;
    } else {
        lc = log_context;
    }


    if (log_level > lc->log_limit) return;

    if ( NULL == lc->log_to) {
#if defined(LINUX) || defined(MACOSX)
        if ((log_domain == NULL) || (log_domain[0] == '\0')) {
            syslog(LOG_INFO, "%s", message);
        } else {
            syslog(LOG_INFO, "%s: %s\n", log_domain, message);
        }
#endif
        return;
    } else {
        if ((log_domain == NULL) || (log_domain[0] == '\0')) {
            fprintf(lc->log_to, "%s\n", message);
        } else {
            fprintf(lc->log_to, "%s: %s\n", log_domain, message);
        }
        fflush(lc->log_to);
        return;
    }
}



// TODO: This should adds '*' subscriptions to all parts that are not specified
static gboolean _add_filter_subscription(const char * resource_pattern) 
{
    char hab_type[BIONET_NAME_COMPONENT_MAX_LEN];
    char hab_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char node_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];
    int r;

    r = bionet_split_name_components_r(resource_pattern, hab_type, hab_id, node_id, resource_id);
    if (r != 0) {
        // a helpful error message has already been logged
        return FALSE;
    }

    // Add hab subscription
    if (strlen(hab_type) && strlen(hab_id)) {
	int hab_name_size = strlen(hab_type) + strlen(hab_id) + 2;
	char *hab_name = malloc(hab_name_size);
	snprintf(hab_name, hab_name_size, "%s.%s", hab_type, hab_id);
	
        hab_list_name_patterns = strv_append(hab_list_name_patterns, hab_name);
        if (NULL == hab_list_name_patterns) {
            free(hab_name);
            return FALSE;
        }
    } else {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "_add_filter_subscription: length of hab_type or hab_id is 0: %s",
	      resource_pattern);
	return FALSE;
    }

    // Add node subscription
    if (strlen(node_id)) {
	int node_name_size = strlen(hab_type) + strlen(hab_id) + strlen(node_id) + 3;
	char *node_name = malloc(node_name_size);
	snprintf(node_name, node_name_size, "%s.%s.%s", hab_type, hab_id, node_id);
	
        node_list_name_patterns = strv_append(node_list_name_patterns, node_name);
        if (NULL == node_list_name_patterns) {
            free(node_name);
            return FALSE;
        }
    } else {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "_add_filter_subscription: length of node_id is 0: %s",
	      resource_pattern);
	return TRUE;
    }

    if (strlen(resource_id)) {
	int resource_name_size = strlen(hab_type) + strlen(hab_id) + strlen(node_id) + strlen(resource_id) + 4;
	char *resource_name = malloc(resource_name_size);
	snprintf(resource_name, resource_name_size, "%s.%s.%s:%s", hab_type, hab_id, node_id, resource_id);
	
        resource_name_patterns = strv_append(resource_name_patterns, resource_name);
        if (NULL == resource_name_patterns) {
            free(resource_name);
            return FALSE;
        }
    } else {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "_add_filter_subscription: length of resource_id is 0: %s",
	      resource_pattern);
	return TRUE;
    }

    return TRUE;
}

int parse_keyfile_options(const char * filename, file_opt_t * file_options)
{
    int errors = 0;
    GError *error = NULL;
    GKeyFile *keyfile;
    GKeyFileFlags flags;

    /* read the DMM ini file */
    keyfile = g_key_file_new ();
    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;
    
    /* Load the GKeyFile from keyfile.conf or return. */
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Loading configuration from keyfile %s", filename);

    if (!g_key_file_load_from_file (keyfile, filename, flags, &error)) {
	g_log (BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "%s %s", error->message, filename);
        return -1;
    }
    g_clear_error(&error);

    file_opt_t * file_opt;
    for(file_opt = file_options; file_opt->key; file_opt++) {
        GError *error = NULL;

        switch(file_opt->type) {
            case G_OPTION_ARG_STRING:
            case G_OPTION_ARG_FILENAME:
            {
                char * str = g_key_file_get_string(keyfile, file_opt->section, file_opt->key, &error);
                if ( NULL == str ) {
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
                            "Failed to get %s from the file %s: %s", 
                            file_opt->name, bdm_config_file, error->message);
                    errors++;
                } else {
                    *(char **)file_opt->val_dest = str;
                }
                break;
            }

            case G_OPTION_ARG_STRING_ARRAY:
            case G_OPTION_ARG_FILENAME_ARRAY:
            {
                size_t count;
                char ** strv = g_key_file_get_string_list(keyfile, file_opt->section, file_opt->key, &count, &error);
                if ( NULL == strv ) {
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
                            "Failed to get %s list from the file %s: %s", 
                            file_opt->name, bdm_config_file, error->message);
                    errors++;
                } else {
                    *(char ***)file_opt->val_dest = strv;
                }
                break;
            }

            case G_OPTION_ARG_INT:
            {
                int ival = g_key_file_get_integer(keyfile, file_opt->section, file_opt->key, &error);
                if ( error ) {
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
                            "Failed to get %s from the file %s: %s", 
                            file_opt->name, bdm_config_file, error->message);
                    errors++;
                } else {
                    *(int*)file_opt->val_dest = ival;
                }
                break;
            }

            default :
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
                        "Bad type for %s", file_opt->key);
                errors++;
                break;
        }
        g_clear_error(&error);
    }

    return errors;
}



int main(int argc, char *argv[]) {
    //
    // we'll be using glib, so capture its log messages
    //
    int switch_to_syslog = 1;

    bdm_log_context_t lc = {
        log_to: stdout,
        log_limit: G_LOG_LEVEL_INFO
    };

    //bionet_log_use_default_handler(&lc);
    (void)g_log_set_default_handler(bdm_glib_log_handler, &lc);


    int i = 0;
    int c;

    int need_bp = 0;

    struct option long_options[] = {
        { "bdm-config-file", 1, 0, 'x' },

        // Terminating NULL;
        { 0, 0, 0, 0}
    };

    opterr = 0; // We don't care about unknown options...
    while(1) {
	c= getopt_long(argc, argv, "-x:", long_options, &i);
	if ((-1) == c) {
	    break;
	}

	switch (c) {
            case 'x':
                bdm_config_file = optarg;
                break;
            default:
                break;
	}
    }
    opterr = 1;
    
    /* reset getopt_long()'s internal state so that we can call it again later */
    optind = 1;


    /* Read the config file, and populate all options */
    parse_keyfile_options(bdm_config_file, fileopts);


    /* Parse the command-line args to override the config file */
    GOptionContext *option_context = g_option_context_new(NULL);
    {
        GError *error = NULL;
        g_option_context_set_summary(option_context, help_summary);
        g_option_context_set_description(option_context, help_description);
        g_option_context_add_main_entries (option_context, entries, NULL);

        if (!g_option_context_parse (option_context, &argc, &argv, &error))
        {
            g_print ("option parsing failed: %s\n", error->message);
            exit (1);
        }
    }
    if(unparsed_args){
        printf("Extra options:");
        for(i=0; i<g_strv_length(unparsed_args); i++) {
            printf(" %s", unparsed_args[i]);
        }
        printf("\n\n");
        puts(g_option_context_get_help(option_context, 1, NULL));
        return 1;
    }

    //
    // Now, interperet the options we've gathered
    //


    if ( keep_self_stats ) {
        ignore_self = 0;
    }


    if(filter_patterns) {
        int len = g_strv_length(filter_patterns);
        for(i=0; i<len; i++) {
            if(!_add_filter_subscription(filter_patterns[i])){
                exit(1);
            }
        }
    }

    // Parse all sync-config files

    if(sync_cfg_file_list) {
        char ** pval;
        for(pval=sync_cfg_file_list; *pval; pval++) {
            sync_sender_config_t * sync_config = NULL;
            sync_config = read_config_file(*pval);
            if (NULL == sync_config) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
                      "Failed to parse sync config file %s", *pval);
                return 1;
            }
            sync_config->last_entry_end_seq = -1;

            if(sync_config->method == BDM_SYNC_METHOD_ION) {
#if ENABLE_ION
                need_bp++;
#else
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
                      "Bad config file '%s': BDM Syncronization over DTN was disabled at compile time.", *pval);
                return (1);
#endif
            }
            sync_config_list = g_slist_append(sync_config_list, sync_config);
            break;
        }
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
		  "Starting BDM-DTN sync receiver");
    }


    // If any DTN options were specified, make sure support is enabled
#if ! ENABLE_ION
    if(enable_dtn_sync_receiver
    || dtn_endpoint_id 
    || ion_key)
    {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
              "DTN capabilities were disabled at compile time.");
        return (-1);
    }
#endif



    if ( bdm_port < 0 || bdm_port > USHRT_MAX) {
        g_warning("invalid tcp port specified: '%d'", bdm_port); 
        return 1;
    }

    if ( enable_debug ) {
        lc.log_limit = G_LOG_LEVEL_DEBUG;
    }

    if ( log_file ) {
        FILE* log_dest = stdout;
        if(strcmp("-", log_file)) {
            log_dest = fopen(log_file, "a");
        }
        if( NULL == log_dest ) {
            fprintf(stderr, "Unable to open log file '%s' for appending: %s",
                    log_file, strerror(errno));
            return 1;
        }
        switch_to_syslog = 0;

        lc.log_to = log_dest;
    }

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Using database file %s", database_file);

    /* debug print the HABs subscribed to */
    char ** pPat;
    if(hab_list_name_patterns){
        for (pPat = hab_list_name_patterns; *pPat; pPat++) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
                  "Subscribing to HABs matching: %s", *pPat);
        }
    }

    /* debug print the Nodes subscribed to */
    if(node_list_name_patterns){
        for (pPat = node_list_name_patterns; *pPat; pPat++) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
                  "Subscribing to Nodes matching: %s", *pPat);
        }
    }

    /* debug print the Resources subscribed to */
    if(resource_name_patterns){
        for (pPat = resource_name_patterns; *pPat; pPat++) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
                  "Subscribing to Resources matching: %s", *pPat);
        }
    }



    if ((require_security) && (security_dir == NULL)) {
        puts(g_option_context_get_help(option_context, 1, NULL));
	g_error("Security required, but no secutiry dir provided.");
	return 1;
    }

    g_thread_init(NULL);

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

    if ((main_db = db_init(database_file?database_file:DB_NAME)) == NULL) {
        // an informative error message has already been logged by db_init()
        exit(1);
    }

    if (keep_self_stats) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Requested to keep stats on ts BDM HAB, but --bdm-stats was not enabled so this BDM is not generating stats.");
    }

    // Add self as bdm to db
    db_insert_bdm(main_db, bionet_bdm_get_id(this_bdm), NULL);


   

    //
    // create the main loop, using the default context, and mark it as "running"
    //

    bdm_main_loop = g_main_loop_new(NULL, TRUE);


    // start the BDM HAB
    if (bdm_stats_interval) {
        bdm_hab = start_stat_hab(bdm_id, &hab_fd);
	if (NULL == bdm_hab) {
	    exit(1);
        }
	
	// watch the hab fd 
        GIOChannel *ch;

        ch = g_io_channel_unix_new(hab_fd);
        g_io_add_watch(ch, G_IO_IN, hab_readable_handler, GINT_TO_POINTER(hab_fd));

	g_timeout_add(bdm_stats_interval * 1000, update_stat_hab, bdm_hab);
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
			    bdm_cal_callback, libbdm_cal_topic_matches, NULL, 0);
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

    if (enable_dtn_sync_receiver) {
	if (NULL == dtn_endpoint_id) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
		  "DTN Sync Receiver requested, but no DTN endpoint ID specified.");
	    return (-1);
	}

        need_bp++;
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

    if ( switch_to_syslog) {
        openlog(NULL, LOG_PID, LOG_USER);
        lc.log_to = NULL; // Syslog
    }

#if ENABLE_ION
    if(need_bp) {
        if (load_ion() != 0) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "ION shared memory base key requested, but can't load ION libraries, aborting");
            return 1;
        }


        if(ion_key){
#if         HAVE_SM_SET_BASEKEY
                (*bdm_bp_funcs.sm_set_basekey)(ion_key);
#           else	
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
                      "Setting ION basekey not supported");
                return (1);
#           endif // HAVE_SM_SET_BASEKEY
        }


        if ((*bdm_bp_funcs.bp_attach)() < 0)
        {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                "Can't attach to BP.");
            return -1;
        }
    }
#endif // ENABLE_ION



    if (sync_config_list || enable_dtn_sync_receiver) {
        make_shutdowns_clean(1);
    } else {
        make_shutdowns_clean(0);
    }


    //create a config for each sync sender configuration
    for (i = 0; i < g_slist_length(sync_config_list); i++) {
	sync_sender_config_t * sync_config = NULL;

	//init the latest entry end time for the config
	sync_config = g_slist_nth_data(sync_config_list, i);
	if (sync_config) {
	    sync_config->db = db_init(database_file);
	    sync_config->last_entry_end_seq = 
                db_get_last_sync_seq(sync_config->db, 
                    sync_config->sync_recipient);

	} else {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
		  "Config number %d is not in the list.", i);
	}
    }

    GThread * thread = NULL;
    if (g_slist_length(sync_config_list)) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
	      "Starting sync thread");
	thread = g_thread_create(sync_thread, sync_config_list, TRUE, NULL);

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
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
	      "DTN Sync Receiver starting. DTN endpoint ID: %s", dtn_endpoint_id);



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

    disconnect_from_bionet(NULL);
    cal_server.shutdown(libbdm_cal_handle);

    // shutdown the database after processing all requests
    db_shutdown(main_db);

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
        "Bionet Data Manager shut down cleanly");

    if(lc.log_to) {
        fclose(lc.log_to);
    }
    return 0;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
