
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
#include "config.h"
#include "cal-server.h"
#include "hardware-abstractor.h"

#define BDM_OPTIONS_STRING "?vedbkt::f:h:I:i:n:p:r:s:c:o:x:"
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
    {"keep-stats",         0, 0, 'k'},
    {"no-resources",       0, 0, 'u'},
    {"bdm-config-file",    1, 0, 'x'},
    {0, 0, 0, 0} //this must be last in the list
};


GMainLoop *bdm_main_loop = NULL;


#define DB_NAME "bdm.db"

int database_file_set = 0;

void * libbdm_cal_handle = NULL;
bionet_bdm_t * this_bdm = NULL;

GSList * sync_config_list = NULL;
static GSList * sync_thread_list = NULL;

static int hab_fd = -1;
int start_hab = 0;
static unsigned int bdm_stats = 300;

int bdm_shutdown_now = 0;
#ifdef ENABLE_ION
client_t dtn_thread_data = {0};
#endif

uint32_t num_sync_datapoints = 0;
uint32_t num_bionet_datapoints = 0;
uint32_t num_db_commits = 0;

extern int no_resources;

extern struct timeval dp_ts_accum;
extern struct timeval db_accum;

bionet_hab_t * bdm_hab = NULL;
int ignore_self = 1;

char * bdm_config_file = "/etc/bdm.ini";

extern int optind;

#if ENABLE_ION

static int bp_attached = 0;
bdm_bp_funcs_t bdm_bp_funcs;


int load_ion(void) {
    void *libbp_handle;
    char *error;
    static int loaded_ion = 0;

    if (loaded_ion) return 0;

    libbp_handle = dlopen("libbp.so", RTLD_NOW);
    if (libbp_handle == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "failed to dlopen libbp.so: %s", dlerror());
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
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find sm_set_basekey() in libbp.so: %s", error);
        return -1;
    }

    bdm_bp_funcs.bp_attach = (bp_attach_t)dlsym(libbp_handle, "bp_attach");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find bp_attach() in libbp.so: %s", error);
        return -1;
    }

    bdm_bp_funcs.bp_interrupt = (bp_interrupt_t)dlsym(libbp_handle, "bp_interrupt");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find bp_interrupt() in libbp.so: %s", error);
        return -1;
    }

    bdm_bp_funcs.bp_open = (bp_open_t)dlsym(libbp_handle, "bp_open");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find bp_open() in libbp.so: %s", error);
        return -1;
    }

    bdm_bp_funcs.bp_close = (bp_close_t)dlsym(libbp_handle, "bp_close");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find bp_close() in libbp.so: %s", error);
        return -1;
    }

    bdm_bp_funcs.bp_get_sdr = (bp_get_sdr_t)dlsym(libbp_handle, "bp_get_sdr");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find bp_get_sdr() in libbp.so: %s", error);
        return -1;
    }

    bdm_bp_funcs.bp_send = (bp_send_t)dlsym(libbp_handle, "bp_send");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find bp_send() in libbp.so: %s", error);
        return -1;
    }

    bdm_bp_funcs.bp_receive = (bp_receive_t)dlsym(libbp_handle, "bp_receive");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find bp_receive() in libbp.so: %s", error);
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
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find bp_release_delivery() in libbp.so: %s", error);
        return -1;
    }

    bdm_bp_funcs.Sdr_malloc = (Sdr_malloc_t)dlsym(libbp_handle, "Sdr_malloc");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find Sdr_malloc() in libbp.so: %s", error);
        return -1;
    }

    bdm_bp_funcs.sdr_begin_xn = (sdr_begin_xn_t)dlsym(libbp_handle, "sdr_begin_xn");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find sdr_begin_xn() in libbp.so: %s", error);
        return -1;
    }

    bdm_bp_funcs.sdr_cancel_xn = (sdr_cancel_xn_t)dlsym(libbp_handle, "sdr_cancel_xn");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find sdr_cancel_xn() in libbp.so: %s", error);
        return -1;
    }

    bdm_bp_funcs.sdr_end_xn = (sdr_end_xn_t)dlsym(libbp_handle, "sdr_end_xn");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find sdr_end_xn() in libbp.so: %s", error);
        return -1;
    }

    bdm_bp_funcs.Sdr_write = (Sdr_write_t)dlsym(libbp_handle, "Sdr_write");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find Sdr_write() in libbp.so: %s", error);
        return -1;
    }

    bdm_bp_funcs.zco_create = (zco_create_t)dlsym(libbp_handle, "zco_create");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find zco_create() in libbp.so: %s", error);
        return -1;
    }

    bdm_bp_funcs.zco_append_extent = (zco_append_extent_t)dlsym(libbp_handle, "zco_append_extent");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find zco_append_extent() in libbp.so: %s", error);
        return -1;
    }

    bdm_bp_funcs.zco_start_receiving = (zco_start_receiving_t)dlsym(libbp_handle, "zco_start_receiving");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find zco_start_receiving() in libbp.so: %s", error);
        return -1;
    }

    bdm_bp_funcs.zco_receive_source = (zco_receive_source_t)dlsym(libbp_handle, "zco_receive_source");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find zco_receive_source() in libbp.so: %s", error);
        return -1;
    }

    bdm_bp_funcs.zco_stop_receiving = (zco_stop_receiving_t)dlsym(libbp_handle, "zco_stop_receiving");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find zco_stop_receiving() in libbp.so: %s", error);
        return -1;
    }

    bdm_bp_funcs.writeErrMemo = (writeErrMemo_t)dlsym(libbp_handle, "writeErrMemo");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find writeErrMemo() in libbp.so: %s", error);
        return -1;
    }

    bdm_bp_funcs.writeErrmsgMemos = (writeErrmsgMemos_t)dlsym(libbp_handle, "writeErrmsgMemos");
    error = dlerror();
    if (error != NULL)  {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "couldn't find writeErrmsgMemos() in libbp.so: %s", error);
        return -1;
    }

    loaded_ion = 1;
    return 0;
}

#endif




void usage(void) {
    printf(
	"'bionet-data-manager' records Bionet traffic to a database.\n"
	"\n"
	"Note: options should be specified in the bdm.ini file. Command line options override them.\n"
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
	" -f,--file /Path/to/database/file.db            Full path of database file (bdm.db). Overrides\n"
	"                                                --bdm-config-file\n"
        "                                                If the file doesn't exist, it will be created\n"
	" -h,--hab,--habs \"HAB-Type.Hab-ID\"              Subscribe to a HAB list. Appends to ones"
	"                                                listed in bdm.ini file.\n"
	" -i,--id <ID>                                   ID of Bionet Data Manager\n"
#if ENABLE_ION
#if HAVE_SM_SET_BASEKEY
	" --ion-key <int>                                Alternate ION key to use if syncing over ION\n"       
#endif
#endif
	" -k,--keep-stats                                Also count datapoints generated by this BDM"
	" -n,--node,--nodes \"HAB-Type.HAB-ID.Node-ID\"    Subscribe to a Node list. Appends to ones"
	"                                                listed in bdm.ini file.\n"
#if ENABLE_ION
	" -o,--dtn-endpoint-id <ID>                      DTN endpoint ID (ex: ipn:1.2)\n"
#endif
        " -p,--port <port>                               Alternate BDM Client port. Default: %u\n"
	" -r,--resource,--resources \"HAB-Type.HAB-ID.Node-ID:Resource-ID\"\n"
	"                                                Subscribe to Resource values. Appends to ones"
	"                                                listed in bdm.ini file.\n"
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
	" -x,--bdm-config-file <FILE>                    BDM configuration file to specify location\n"
	"                                                of database file and subscriptions.\n"
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

    return cal_server.read(libbdm_cal_handle, &timeout, 0);
}


gboolean update_hab(gpointer usr_data) {
    uint32_t num_real = num_bionet_datapoints;
    uint32_t local_last = 0;
    uint32_t dtn_last = 0;
    struct timeval tv;
    bionet_hab_t * hab;
    bionet_node_t * node;
    bionet_resource_t * dtn;
    bionet_resource_t * local;
    bionet_resource_t * dtn_rate;
    bionet_resource_t * local_rate;
    bionet_resource_t * dp_latency;
    bionet_resource_t * db_latency;
    bionet_resource_t * commit;

    if (gettimeofday(&tv, NULL)) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Failed to get time of day.");
    }


    hab = (bionet_hab_t *)usr_data;
    if (NULL == hab) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "NULL HAB passed in.");
	goto ret;
    }

    node = bionet_hab_get_node_by_id(hab, "Statistics");
    if (NULL == node) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "Statistics node not found.");
	goto ret;
    }

    dtn = bionet_node_get_resource_by_id(node, "Number-of-Datapoints-over-DTN-Recorded");
    if (NULL == dtn) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "Num DTN Datapoints resource not found");
	return 1;
    } else {
	uint32_t cur;
	struct timeval tvtmp;
	bionet_resource_get_uint32(dtn, &cur, &tvtmp);
	dtn_last = cur;
	if (cur != num_sync_datapoints) {
	    bionet_resource_set_uint32(dtn, num_sync_datapoints, &tv);
	}
    }

    local = bionet_node_get_resource_by_id(node, "Number-of-Local-Datapoints-Recorded");
    if (NULL == local) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "Num Local Datapoints resource not found");
	return 1;
    } else {
	uint32_t cur;
	struct timeval tvtmp;
	bionet_resource_get_uint32(local, &cur, &tvtmp);
	local_last = cur;
	if (cur != num_real) {
	    bionet_resource_set_uint32(local, num_real, &tv);
	}
    }

    dtn_rate = bionet_node_get_resource_by_id(node, "DTN-Datapoints-Per-Second");
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
    }

    local_rate = bionet_node_get_resource_by_id(node, "Local-Datapoints-Per-Second");
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
    }

    dp_latency = bionet_node_get_resource_by_id(node, "Datapoint-Latency-ms");
    if (NULL == local_rate) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "Datapoint Latency resource not found");
	return 1;
    } else {
	if (local_last > 0) {
	    uint32_t latency = (dp_ts_accum.tv_sec * 1000) + (dp_ts_accum.tv_usec/1000);
	    latency /= (num_real/local_last);
	    bionet_resource_set_uint32(dp_latency, latency, &tv);
	}
	dp_ts_accum.tv_sec = 0;
	dp_ts_accum.tv_usec = 0;
    }

    db_latency = bionet_node_get_resource_by_id(node, "DB-Write-Latency-ms");
    if (NULL == local_rate) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "DB Write Latency resource not found");
	return 1;
    } else {
	if (local_last > 0) {
	    uint32_t latency = (db_accum.tv_sec * 1000) + (db_accum.tv_usec/1000);
	    latency /= (num_real/local_last);
	    bionet_resource_set_uint32(db_latency, latency, &tv);
	}
	db_accum.tv_sec = 0;
	db_accum.tv_usec = 0;
    }

    /* DB Commits */
    commit = bionet_node_get_resource_by_id(node, "Number-of-DB-Commits");
    if (NULL == commit) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "Num DB Commits resource not found");
	return 1;
    } else {
	uint32_t cur;
	struct timeval tvtmp;
	bionet_resource_get_uint32(commit, &cur, &tvtmp);
	if (cur != num_db_commits) {
	    bionet_resource_set_uint32(commit, num_db_commits, &tv);
	}
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

    bionet_log_use_default_handler(&lc);

    int tcp_sync_recv_port = BDM_SYNC_PORT;
    int bdm_port = BDM_PORT;
    int enable_tcp_sync_receiver = 0;
    int enable_dtn_sync_receiver = 0;
    char * database_file = NULL;

    sync_sender_config_t * sync_config = NULL;
    char * sync_sender_config_file_name = NULL;
    long key;
    int i = 0;
    int c;

    GKeyFile *keyfile;
    GKeyFileFlags flags;
    GError *error = NULL;
    gsize length;
    int no_config_file = 0;

    while(1) {
	c= getopt_long(argc, argv, BDM_OPTIONS_STRING, long_options, &i);
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
    
    /* reset getopt_long()'s internal state so that we can call it again later */
    optind = 1;

    /* read the DMM ini file */
    keyfile = g_key_file_new ();
    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;
    
    /* Load the GKeyFile from keyfile.conf or return. */
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Loading configuration from keyfile %s", bdm_config_file);
    if (!g_key_file_load_from_file (keyfile, bdm_config_file, flags, &error)) {
	g_log (BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "%s %s", error->message, bdm_config_file);
	no_config_file = 1;
    }
    g_clear_error(&error);

    /* Database File */
    if (0 == no_config_file) {
	database_file = g_key_file_get_string(keyfile, "BDM", "DBFILE", &error);
	if (NULL == database_file) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Failed to get DB File name from the file %s. %s", 
		  bdm_config_file, error->message);
	    database_file = DB_NAME;
	}
    } else {
	database_file = DB_NAME;
	goto skip;
    }
    g_clear_error(&error);

    /* HAB subscriptions */
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Collecting HAB subscriptions from %s", bdm_config_file);
    hab_list_name_patterns = g_key_file_get_string_list(keyfile, "BDM", "HABS", &length, &error);
    if ((NULL == hab_list_name_patterns) || (0 >= length)) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	      "Failed to get HAB subscriptions from the file %s.", bdm_config_file);
    } else {
	hab_list_index = (int)length;
    }
    g_clear_error(&error);

    /* Node subscriptions */
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Collecting Node subscriptions from %s", bdm_config_file);
    node_list_name_patterns = g_key_file_get_string_list(keyfile, "BDM", "NODES", &length, &error);
    if ((NULL == node_list_name_patterns) || (0 >= length)) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	      "Failed to get Node subscriptions from the file %s.", bdm_config_file);
    } else {
	node_list_index = (int)length;
    }
    g_clear_error(&error);

    /* Resource subscriptions */
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Collecting Resource subscriptions from %s", bdm_config_file);
    resource_name_patterns = g_key_file_get_string_list(keyfile, "BDM", "RESOURCES", &length, &error);
    if ((NULL == resource_name_patterns) || (0 >= length)) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	      "Failed to get Resource subscriptions from the file %s.", bdm_config_file);
    } else {
	resource_index = (int)length;
    }
    g_clear_error(&error);
    
    /* BDM ID */
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Collecting ID from %s", bdm_config_file);
    bdm_id = g_key_file_get_string(keyfile, "BDM", "ID", &error);
    if (NULL == bdm_id) {
	 g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Failed to get BDM ID from the file %s. %s", 
	       bdm_config_file, error->message);
    }
    g_clear_error(&error);

    /* BDM Stats Interval */
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Collecting BDM Stats interval from %s", bdm_config_file);
    bdm_stats = g_key_file_get_integer(keyfile, "BDM", "BdmStats", &error);
    if ((NULL == error) && (0 != bdm_stats)) {
	start_hab = 1;
    } else {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "BdmStats key not found in %s so not starting the BDM Stats HAB. %s", 
	      bdm_config_file, error->message);
    }
    g_clear_error(&error);

    /* Sync Sender Config File Name */
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Collecting Sync Sender Config from %s", bdm_config_file);
    sync_sender_config_file_name = g_key_file_get_string(keyfile, "BDM", "SyncSenderConfig", &error);
    if (error) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Error reading SyncSenderConfig key in %s. %s",
	      bdm_config_file, error->message);
    } else {
	sync_config = read_config_file(sync_sender_config_file_name);
	if (NULL == sync_config) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
		  "Failed to parse config file %s", sync_sender_config_file_name);
	}
	sync_config->last_entry_end_seq = -1;
	sync_config->last_entry_end_seq_metadata = -1;
	if (sync_config->method == BDM_SYNC_METHOD_ION) {
#if ENABLE_ION
            if (!bp_attached) {
                if (load_ion() != 0) {
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "DTN syncing requested, but can't load ION libraries, aborting");
                    return 1;
                }

                if ((*bdm_bp_funcs.bp_attach)() < 0)
                {
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
                          "Can't attach to BP, but DTN syncing requested");
                    return 1;
                }
                bp_attached++;
            }
#else
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
		  "Bad config file '%s': BDM Syncronization over DTN was disabled at compile time.", 
		  sync_sender_config_file_name);
	    return (1);
#endif
	}
	sync_config_list = g_slist_append(sync_config_list, sync_config);
    }
    g_clear_error(&error);

    /* DTN Sync Receiver */
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Collecting DTN Sync Receiver from %s", bdm_config_file);
    enable_dtn_sync_receiver = (int)g_key_file_get_boolean(keyfile, "BDM", "DtnSyncReceiver", &error);
    if (error) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Error reading DtnSyncReceiver key in %s. %s",
	      bdm_config_file, error->message);
    } else if (enable_dtn_sync_receiver) {
#if ENABLE_ION
	enable_dtn_sync_receiver = 1;
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	      "Starting BDM-DTN sync receiver");
#else
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
	      "BDM Syncronization over DTN was disabled at compile time.");
	return (1);
#endif
    }
    g_clear_error(&error);

    /* Security Directory*/
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Collecting Security Directory from %s", bdm_config_file);
    security_dir = g_key_file_get_string(keyfile, "BDM", "SecurityDir", &error);
    if (error) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Error reading SecurityDir key in %s. %s", 
	      bdm_config_file, error->message);
    } 
    g_clear_error(&error);
   
    /* Require Security */
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Collecting Security Requirements from %s", bdm_config_file);
    require_security = (int)g_key_file_get_boolean(keyfile, "BDM", "RequireSecurity", &error);
    if (error) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Error reading RequireSecurity key in %s. %s", 
	      bdm_config_file, error->message);
    }
    g_clear_error(&error);

    /* ION Key */
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Collecting ION Key from %s", bdm_config_file);
    key = (long)g_key_file_get_integer(keyfile, "BDM", "IonKey", &error);
    if (error) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Error reading IonKey key in %s. %s", 
	      bdm_config_file, error->message);
    } else {
#if HAVE_SM_SET_BASEKEY
        if (load_ion() != 0) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "ION SM basekey supplied, but can't load ION libraries, aborting");
            return 1;
        }

        (*bdm_bp_funcs.sm_set_basekey)(key);
#else	
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
	      "Setting ION basekey not supported");
	return (1);
#endif // HAVE_SM_SET_BASEKEY
    }
    g_clear_error(&error);

    /* Keep Stats for BDM HAB too */
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Collecting Keep Stats from %s", bdm_config_file);
    int keep_stats = (int)g_key_file_get_boolean(keyfile, "BDM", "KeepStats", &error);
    if (error) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Error reading KeepStats key in %s. %s", 
	      bdm_config_file, error->message);
    } else if (keep_stats) {
	ignore_self = 0;
    }
    g_clear_error(&error);

#if ENABLE_ION
    /* DTN Endpoint ID */
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Collecting DTN Endpoint ID from %s", bdm_config_file);
    dtn_endpoint_id = g_key_file_get_string(keyfile, "BDM", "DtnEndpointId", &error);
    if (error) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Error reading DtnEndpointId key in %s. %s", 
	      bdm_config_file, error->message);
    } 
    g_clear_error(&error);
#endif

    /* BDM Port */
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Collecting BDM Port from %s", bdm_config_file);
    bdm_port = g_key_file_get_integer(keyfile, "BDM", "BdmPort", &error);
    if (error) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Error reading BdmPort key in %s. %s", 
	      bdm_config_file, error->message);
	bdm_port = BDM_PORT;
    } else {
	if (bdm_port >= USHRT_MAX) {
	    g_warning("invalid tcp port specified: '%d'", bdm_port);
	    bdm_port = BDM_PORT;
	}
    }
    g_clear_error(&error);

    /* Enable TCP Sync Receiver */
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Collecting TCP Sync Receiver from %s", bdm_config_file);
    enable_tcp_sync_receiver = (int)g_key_file_get_boolean(keyfile, "BDM", "TcpSyncReceiver", &error);
    if (error) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Error reading TcpSyncReceiver key in %s. %s", 
	      bdm_config_file, error->message);
    }
    g_clear_error(&error);

    if (enable_tcp_sync_receiver) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Collecting TCP Sync Receiver Port from %s", bdm_config_file);
	tcp_sync_recv_port = g_key_file_get_integer(keyfile, "BDM", "TcpSyncReceiverPort", &error);
	if (error) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Error reading BdmPort key in %s. %s", 
		  bdm_config_file, error->message);
	} else {
	    if (tcp_sync_recv_port >= USHRT_MAX) {
		g_warning("invalid tcp port specified: '%d'", tcp_sync_recv_port);
		tcp_sync_recv_port = BDM_SYNC_PORT;
	    }
	}
    }
    g_clear_error(&error);

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Collecting No Resources from %s", bdm_config_file);
    no_resources = (int)g_key_file_get_boolean(keyfile, "BDM", "NoResources", &error);
    if (error) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Error reading NoResources key in %s. %s", 
	      bdm_config_file, error->message);
    }
    g_clear_error(&error);

    //
    // parse command-line arguments
    //
skip:
    i = 0;
    while(1) {
	c= getopt_long(argc, argv, BDM_OPTIONS_STRING, long_options, &i);
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

            if(sync_config->method == BDM_SYNC_METHOD_ION) {
#if ENABLE_ION
                if (!bp_attached) {
                    if (load_ion() != 0) {
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "DTN syncing requested, but can't load ION libraries, aborting");
                        return 1;
                    }

                    if ((*bdm_bp_funcs.bp_attach)() < 0)
                    {
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
                            "Can't attach to BP, but DTN syncing requested");
                        return 1;
                    }
                    bp_attached++;
                }
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
	    require_security++;
	    break;

	case 'f':
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Setting database file to %s", optarg);
	    database_file = optarg;
	    break;

	case 'h':
	    if (hab_list_index < MAX_SUBSCRIPTIONS) {
		hab_list_name_patterns = realloc(hab_list_name_patterns, sizeof(gchar *) * (hab_list_index + 1));
		if (NULL == hab_list_name_patterns) {
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Out of memory, cannot allocate hab_list_name_patterns");
		    return 1;
		}
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

            if (load_ion() != 0) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "ION shared memory base key requested, but can't load ION libraries, aborting");
                return 1;
            }

            key = strtoul(optarg, &endptr, 10);
            if(endptr > optarg 
            && endptr[0] == '\0')
            {
                (*bdm_bp_funcs.sm_set_basekey)(key);
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

	case 'k':
	    ignore_self = 0;
	    break;

	case 'n':
	    if (node_list_index < MAX_SUBSCRIPTIONS) {
		node_list_name_patterns = realloc(node_list_name_patterns, sizeof(gchar *) * (node_list_index + 1));
		if (NULL == node_list_name_patterns) {
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Out of memory, cannot allocate node_list_name_patterns");
		    return 1;
		}
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
		resource_name_patterns = realloc(resource_name_patterns, sizeof(gchar *) * (resource_index + 1));
		if (NULL == resource_name_patterns) {
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Out of memory, cannot allocate resource_name_patterns");
		    return 1;
		}
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
	    
	case 'x':
	    //skip this time, we already got it
	    break;

	default:
            printf("Unknown option\n");
	    usage();
            return 1;
	    break;
	}
    } //while(1)

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Using database file %s", database_file);

    /* debug print the HABs subscribed to */
    for (length = 0; length < hab_list_index; length++) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	      "Subscribing to HABs matching: %s", hab_list_name_patterns[length]);
    }

    /* debug print the Nodes subscribed to */
    for (length = 0; length < node_list_index; length++) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	      "Subscribing to Nodes matching: %s", node_list_name_patterns[length]);
    }

    /* debug print the Resources subscribed to */
    for (length = 0; length < resource_index; length++) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	      "Subscribing to Resources matching: %s", resource_name_patterns[length]);
    }


    if(optind < argc){
        printf("Extra options\n");
        usage();
        return 1;
    }

    if ((require_security) && (security_dir == NULL)) {
	usage();
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

    if (ignore_self == 0) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Requested to keep stats on ts BDM HAB, but --bdm-stats was not enabled so this BDM is not generating stats.");
    }

    // Add self as bdm to db
    db_add_bdm(main_db, bionet_bdm_get_id(this_bdm));


   

    //
    // create the main loop, using the default context, and mark it as "running"
    //

    bdm_main_loop = g_main_loop_new(NULL, TRUE);


    // start the BDM HAB
    if (start_hab) {
	
	bdm_hab = bionet_hab_new("Bionet-Data-Manager", bdm_id);
	if (NULL == bdm_hab) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize Statistics HAB.");
	    exit(1);
	} else {
	    hab_fd = hab_connect(bdm_hab);
	    if (hab_fd == -1) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to connect HAB to Bionet");
		exit(1);
	    }
	    hab_read();
	}

	bionet_node_t * node = bionet_node_new(bdm_hab, "Statistics");
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


	/* Bionet Datapoints Rate */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT32,
				       BIONET_RESOURCE_FLAVOR_SENSOR,
				       "Datapoint-Latency-ms");
	if (NULL == resource) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize Datapoint Latency resource.");
	    exit(1);
	}

	if (bionet_node_add_resource(node, resource)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "Failed to add Datapoint Latency resource to Statistics node");
	    exit(1);
	}


	/* Bionet Datapoints Rate */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT32,
				       BIONET_RESOURCE_FLAVOR_SENSOR,
				       "DB-Write-Latency-ms");
	if (NULL == resource) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize DB Write Latency resource.");
	    exit(1);
	}

	if (bionet_node_add_resource(node, resource)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "Failed to add DB Write Latency resource to Statistics node");
	    exit(1);
	}

        /* DB Commit */
	resource = bionet_resource_new(node, 
				       BIONET_RESOURCE_DATA_TYPE_UINT32,
				       BIONET_RESOURCE_FLAVOR_SENSOR,
				       "Number-of-DB-Commits");
	if (NULL == resource) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to initialize Num DB Commits resource.");
	    exit(1);
	}

	if (bionet_node_add_resource(node, resource)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "Failed to add Num Local Datapoints resource to Statistics node");
	    exit(1);
	}

	if (bionet_resource_set_uint32(resource, num_bionet_datapoints, &tv)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Failed to set Num DB Commits resource value");
	}


	/* Add node */
	if (bionet_hab_add_node(bdm_hab, node)) {
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

	g_timeout_add(bdm_stats * 1000, update_hab, bdm_hab);
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

    //create a config for each sync sender configuration
    for (i = 0; i < g_slist_length(sync_config_list); i++) {
	sync_sender_config_t * sync_config = NULL;

	//init the latest entry end time for the config
	sync_config = g_slist_nth_data(sync_config_list, i);
	if (sync_config) {
	    sync_config->db = db_init(database_file);
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
	if (NULL == dtn_endpoint_id) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
		  "DTN Sync Receiver requested, but no DTN endpoint ID specified.");
	    return (-1);
	}
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
	      "DTN Sync Receiver starting. DTN endpoint ID: %s", dtn_endpoint_id);

        if (load_ion() != 0) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "DTN syncing requested, but can't load ION libraries, aborting");
            return 1;
        }

        if(!bp_attached){
            if ((*bdm_bp_funcs.bp_attach)() < 0)
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

    disconnect_from_bionet(NULL);
    cal_server.shutdown(libbdm_cal_handle);

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
        "Bionet Data Manager shut down cleanly");
    return 0;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
