
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <signal.h>

#include <glib.h>

#if defined(LINUX) || defined(MACOSX)
    #include <netdb.h>
    #include <pwd.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
#endif

#include "bionet.h"
#include "bionet-util.h"
#include "bionet-data-manager.h"
#include "bdm-list-iterator.h"
#include "bdm-db.h"

#define _Min(x,y) ((x)<(y)?(x):(y))

#if ENABLE_ION
char * dtn_endpoint_id = NULL;

// Group ion configs for clarity
typedef struct {
    int basekey; // ION base key, to allow multiple instance on a machine
    Sdr	sdr; // The SDR of the open transaction. NULL when not in transaction
    BpSAP   sap;      // Sender endpoint ID
    Object	bundleZco; // The ZCO that is being appended to.
    size_t  bundle_size; // Running total of all extents appended to bundle
} ion_config_t;

ion_config_t ion;
#endif

GMainLoop * sync_sender_main_loop = NULL;

static int sync_init_connection(sync_sender_config_t * config);
static void sync_cancel_connection(sync_sender_config_t * config);
static int sync_finish_connection(sync_sender_config_t * config);
static int write_data_to_message(const void *buffer, size_t size, void * config_void);
static int write_data_to_socket(const void *buffer, size_t size, void * config_void);
static gboolean sync_check(gpointer data);

#if ENABLE_ION
static int write_data_to_ion(const void *buffer, size_t size, void * config_void);
#endif

static int sync_send_metadata(
    sync_sender_config_t * config,
    int from_seq, 
    int to_seq) 
{
    GPtrArray * bdm_list = NULL;
    char * hab_type;
    char * hab_id;
    char * node_id;
    char * resource_id;
    BDM_Sync_Message_t *sync_message = NULL;
    int r;

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	  "Syncing metadata to %s", config->sync_recipient);

    if (sync_init_connection(config)) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	      "    Unable to make TCP connection to BDM Sync Receiver");
	return -1;
    }

    if (bionet_split_resource_name(config->resource_name_pattern,
				   &hab_type, &hab_id, &node_id, &resource_id)) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "Failed to split resource name pattern: %s", config->resource_name_pattern);
        sync_cancel_connection(config);
        return -1;
    }

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	  "    METADATA for %s.%s.%s:%s from seq %d to %d",
	  hab_type, hab_id, node_id, resource_id, from_seq, to_seq);
    bdm_list = db_get_metadata_bdmlist(config->db, NULL, hab_type, hab_id, node_id, resource_id,
                               NULL, NULL,
			       from_seq, to_seq);

    if (NULL == bdm_list) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "send_sync_metadata(): NULL BDM list from db_get_metadata()");
        sync_cancel_connection(config);
        return -1;
    } else {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	      "    send_sync_metadata(): compiling metadata message from BDM list");
    }

    // sync_message NULL when nothing to send
    md_iter_state_t make_message_state;
    bdm_list_iterator_t iter;

    bdm_sync_metadata_to_asn_setup(bdm_list, &make_message_state, &iter);

    while((sync_message = bdm_sync_metadata_to_asn(&iter, &make_message_state)))
    {
	// send the reply to the client
	asn_enc_rval_t asn_r;
	asn_r = der_encode(&asn_DEF_BDM_Sync_Message, sync_message, 
            write_data_to_message, config);

        ASN_STRUCT_FREE(asn_DEF_BDM_Sync_Message, sync_message);

	if (asn_r.encoded == -1) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                    "send_sync_metadata(): error with der_encode(): %s", 
                    asn_r.failed_type ? asn_r.failed_type->name : "unknown");
            bdm_list_free(bdm_list);
            sync_cancel_connection(config);
            return -1;
	}
    }


    r = sync_finish_connection(config);
    if ( r == 0 ) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
              "    Sync metadata finished");
    } else {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
              "    Sync metadata FAILED");

    }

    bdm_list_free(bdm_list);

    return r;

} /* sync_send_metadata() */


static int count_sync_datapoints(BDM_Sync_Message_t * msg) {

    int n = 0;

    int i;
    for(i=0; i<msg->choice.datapointsMessage.list.count; i++) {
        int j;
        BDMSyncRecord_t * syncRecord = msg->choice.datapointsMessage.list.array[i]; 

        for(j=0; j<syncRecord->syncResources.list.count; j++) {
            n += syncRecord->syncResources.list.array[j]->resourceDatapoints.list.count;
        }
    }

    return n;
}

static int sync_send_datapoints(
    sync_sender_config_t * config,
    int from_seq, 
    int to_seq) 
{
    GPtrArray * bdm_list = NULL;
    char * hab_type;
    char * hab_id;
    char * node_id;
    char * resource_id;
    BDM_Sync_Message_t *sync_message = NULL;
    int r;

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	  "Syncing datapoints");

    if (sync_init_connection(config)) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	      "    Unable to make TCP connection to BDM Sync Receiver");
	return -1;
    }

    if (bionet_split_resource_name(config->resource_name_pattern,
				   &hab_type, &hab_id, &node_id, &resource_id)) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "Failed to split resource name pattern: %s", config->resource_name_pattern);
        goto cleanup;
    }


    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	  "    DATAPOINTS for %s.%s.%s:%s from seq %d to %d",
	  hab_type, hab_id, node_id, resource_id,
	  from_seq, to_seq);
    bdm_list = db_get_resource_datapoints_bdm_list(config->db, NULL, hab_type, hab_id, node_id, resource_id,
					  &config->start_time, &config->end_time,
                                          NULL, NULL,
					  from_seq, to_seq);
    if (NULL == bdm_list) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "send_sync_datapoints(): NULL BDM list from db_get_resource_datapoints()");
	goto cleanup;
    }

    dp_iter_state_t make_message_state;
    bdm_list_iterator_t iter;

    bdm_sync_datapoints_to_asn_setup(bdm_list, &make_message_state, &iter);

    while((sync_message = bdm_sync_datapoints_to_asn(&iter, &make_message_state)))
    {
	// send the reply to the client
	asn_enc_rval_t asn_r;
	asn_r = der_encode(&asn_DEF_BDM_Sync_Message, sync_message, 
            write_data_to_message, config);


	if (asn_r.encoded == -1) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "send_sync_datapoints(): error with der_encode(): %m");
            bdm_list_free(bdm_list);
            ASN_STRUCT_FREE(asn_DEF_BDM_Sync_Message, sync_message);
            goto cleanup;
	}

        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
              "Syncd %d datapoints to %s for seqs [%d,%d]",
              count_sync_datapoints(sync_message),
              config->sync_recipient,
              from_seq, to_seq);
        ASN_STRUCT_FREE(asn_DEF_BDM_Sync_Message, sync_message);
    }
    bdm_list_free(bdm_list);

    r = sync_finish_connection(config);
    if( r != 0 ) {
        goto cleanup;
    }

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
          "    Sync datapoints finished");

    return 0;

cleanup:
    sync_cancel_connection(config);
    return -1;
} /* sync_send_datapoints() */


static int write_data_to_socket(const void *buffer, size_t size, void * config_void) {
    sync_sender_config_t * config = (sync_sender_config_t *)config_void;
    int r;

    r = write(config->fd, buffer, size);

    if( r < 0 ) { 
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "    ERROR writing to %d: %s", config->fd, strerror(errno));
    } else {

        /*
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
              "    %d bytes written", r);
              */

        config->bytes_sent += r;
    }

    return r;
} /* write_data_to_socket() */

#if ENABLE_ION
static int write_data_to_ion(const void *buffer, size_t size, void * config_void) {
    sync_sender_config_t * config = (sync_sender_config_t *)config_void;

    Object	extent;
    Sdr sdr = ion.sdr;
    Object bundleZco = ion.bundleZco;

    if(sdr == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "SDR not initialized for send");
        return -1;
    }

    if(size == 0) {
        return 0;
    }

    if(config->sync_mtu > 0 && config->buf_len + size > config->sync_mtu) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "Bundle would exceed MTU of %d", config->sync_mtu);
        return -1;
    }

    int bytes_to_copy = _Min(size,BP_SEND_BUF_SIZE - config->buf_len);
    char * dst = config->send_buf + config->buf_len;
    memcpy(dst, buffer, bytes_to_copy);
    config->buf_len += bytes_to_copy;

    if(config->buf_len >= BP_SEND_BUF_SIZE) {
        extent = bdm_sdr_malloc(sdr, BP_SEND_BUF_SIZE);
        if (extent == 0)
        {
            (*bdm_bp_funcs.sdr_cancel_xn)(sdr);
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                "No space for ZCO extent.");

            ion.sdr = NULL;
            return -1;
        }

        bdm_sdr_write(sdr, extent, (void*)config->send_buf, BP_SEND_BUF_SIZE);
        (*bdm_bp_funcs.zco_append_extent)(sdr, bundleZco, ZcoSdrSource, extent, 0, BP_SEND_BUF_SIZE);
        ion.bundle_size += BP_SEND_BUF_SIZE;

        config->buf_len = 0;
    }

    return size;
}
#endif


gpointer sync_thread(gpointer config_list) {
    int i;
    sync_sender_config_t * cfg;
    GSList * cfg_list = (GSList *)config_list;


#ifdef ENABLE_ION
    int need_bp = 0;
#endif

    for (i = 0; i < g_slist_length(cfg_list); i++) {
	cfg = g_slist_nth_data(cfg_list, i);
	
       	if (NULL == cfg) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
		  "sync_thread() found NULL config: %d", i); 
	    return NULL;
	}

        db_insert_bdm(cfg->db, cfg->sync_recipient, NULL);

	cfg->last_entry_end_seq = 
	    db_get_last_sync_seq(cfg->db, cfg->sync_recipient);
    
	// One-time setup
#if ENABLE_ION
	if( cfg->method == BDM_SYNC_METHOD_ION ) {
	    if(NULL == dtn_endpoint_id) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "sync-send: No source EID defined. Cannot continue");
		return NULL;
	    }
	    need_bp++;
	}
#endif
    }

#ifdef ENABLE_ION
    if (need_bp) {
        int r;
        
        r = (*bdm_bp_funcs.bp_open)(dtn_endpoint_id, &ion.sap);
        if (r < 0) {
            if (bdm_bp_funcs.bp_add_endpoint != NULL) {
                if ((*bdm_bp_funcs.bp_add_endpoint)(dtn_endpoint_id, NULL) != 0) {
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Can't create own endpoint ('%s')", dtn_endpoint_id);
                    return NULL;
                }
                r = (*bdm_bp_funcs.bp_open)(dtn_endpoint_id, &ion.sap);
            }
            if (r < 0) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Can't open own endpoint ('%s')", dtn_endpoint_id);
                return NULL;
            }
        }
    }
#endif // ENABLE_ION

    GMainContext * this_loop_context = 
        g_main_context_new();

    sync_sender_main_loop = g_main_loop_new(this_loop_context, TRUE);
    
    for (i = 0; i < g_slist_length(cfg_list); i++) {
        GSource * source;
	cfg = g_slist_nth_data(cfg_list, i);

	source = g_timeout_source_new_seconds(cfg->frequency);
        g_source_set_callback(source, sync_check, cfg, NULL);
        g_source_attach(source, this_loop_context);
    }

    g_main_loop_run(sync_sender_main_loop);

    for (i = 0; i < g_slist_length(cfg_list); i++) {
	cfg = g_slist_nth_data(cfg_list, i);
	if (cfg) {
	    sync_sender_config_destroy(cfg);
	}
    }

    sync_sender_main_loop = NULL;

    return NULL;
} /* sync_thread() */

static gboolean sync_check(gpointer data) {
    sync_sender_config_t * cfg = (sync_sender_config_t *)data;
    int curr_seq = 0;


    curr_seq = db_get_latest_entry_seq(cfg->db);
    
    if (curr_seq > cfg->last_entry_end_seq) {
        int start_seq = cfg->last_entry_end_seq + 1;
        int r;

        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Syncing to %s seq [%d,%d]", 
                cfg->sync_recipient, start_seq, curr_seq);
	r = sync_send_metadata(cfg, start_seq, curr_seq);
        if ( r ) return TRUE;
	
	r = sync_send_datapoints(cfg, start_seq, curr_seq);
        if ( r ) return TRUE;

        cfg->last_entry_end_seq = curr_seq;
    }

    return TRUE;
} /* sync_check() */

#if ENABLE_ION
static int sync_init_connection_ion(sync_sender_config_t * config) {
    ion.sdr = (*bdm_bp_funcs.bp_get_sdr)();
	
    (*bdm_bp_funcs.sdr_begin_xn)(ion.sdr);

    ion.bundleZco = (*bdm_bp_funcs.zco_create)(ion.sdr, ZcoSdrSource, 0,
			       0, 0);
    ion.bundle_size = 0;

    (*bdm_bp_funcs.writeErrmsgMemos)();

    return 0;
}
#endif


static void sync_cancel_connection(sync_sender_config_t *config) {
    switch ( config->method ) {
        case BDM_SYNC_METHOD_TCP:
            // nothing to do here
            break;

#if ENABLE_ION
        case BDM_SYNC_METHOD_ION:
            if(ion.sdr) {
                (*bdm_bp_funcs.sdr_cancel_xn)(ion.sdr);
                ion.sdr = NULL;
            }
            break;
#endif
        
        default:
            g_log(
                BDM_LOG_DOMAIN,
                G_LOG_LEVEL_WARNING,
                "sync_cancel_connection(): failed to cancel: unknown method %d",
                config->method
            );
    }
}



/*
 * Read the TCP ack, which is sent once this message has been committed to disk
 */
static int read_ack_tcp(sync_sender_config_t *config) {
    int bytes_to_read;
    int bytes_read = 0;
    asn_dec_rval_t rval;

    char buffer[128];
    int index = 0;
    BDM_Sync_Ack_t sync_ack;
    BDM_Sync_Ack_t * p_sync_ack = &sync_ack;

    memset(&sync_ack, 0, sizeof(sync_ack));

    while (1) {
	bytes_to_read = sizeof(buffer) - index;
	bytes_read = read(config->fd, &buffer[index], bytes_to_read);
	if (bytes_read < 0) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error reading from sync receiver: %s", strerror(errno));
	    return -1;
	}
	if (bytes_read == 0) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "eof from sync receiver");
	    return -1;
	}
	
	index += bytes_read;

        rval = ber_decode(NULL, 
			  &asn_DEF_BDM_Sync_Ack, 
			  (void **)&p_sync_ack, 
			  buffer, 
			  index);

        if (rval.code == RC_OK) {

            if(sync_ack < 0) {
                // The connection worked, but the remote side couldn't save the
                // data 
                return -1;
            }

            switch(sync_ack){
                case BDM_Sync_Ack_datapointAck:
                    db_set_last_sync_seq(config->db, 
                        config->sync_recipient, 
                        config->last_entry_end_seq);

                    break;

                case BDM_Sync_Ack_metadataAck:
                    db_set_last_sync_seq(config->db, 
                        config->sync_recipient, 
                        config->last_entry_end_seq);
                    break;

                default:
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
                        "Unhandled ack value: %ld", sync_ack);
                    return -1;
            }

            return 0;
                        
        } else if (rval.code == RC_WMORE) {
            // ber_decode is waiting for more data, but so far so good
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "ber_decode: waiting for more data");
	    usleep(10000);
        } else if (rval.code == RC_FAIL) {
	    // received invalid junk
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "ber_decode failed to decode the sync receiver's message");
        } else {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "unknown error with ber_decode (code=%d)", rval.code);
        }

        if (rval.consumed > 0) {
            index -= rval.consumed;
            memmove(buffer, &buffer[rval.consumed], index);
        }
    }

    return -1;
}

static int sync_finish_connection_tcp(sync_sender_config_t * config) {
    int rc = 0, r = 0;

    if(config->bytes_sent) {
        // TCP will ack the message in the same connection. Wait for it.
        rc = read_ack_tcp(config);
    }

    config->bytes_sent = 0;

    while((r = close(config->fd)) < 0 && errno == EINTR);

    if(r<0){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "Can't close TCP connection: %m");
        rc = -1;
    }

    return rc;
}


#if ENABLE_ION
static int sync_finish_connection_ion(sync_sender_config_t * config) {

    // Append any bytes in buffer to zco
    if(config->buf_len > 0) {
        Object	extent;
        Object bundleZco = ion.bundleZco;

        extent = bdm_sdr_malloc(ion.sdr, config->buf_len);
        if (extent == 0)
        {
            (*bdm_bp_funcs.sdr_cancel_xn)(ion.sdr);
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                "No space for ZCO extent.");

            ion.sdr = NULL;
            return -1;
        }

        bdm_sdr_write(ion.sdr, extent, (void*)config->send_buf, config->buf_len);
        (*bdm_bp_funcs.zco_append_extent)(ion.sdr, bundleZco, ZcoSdrSource, extent, 0, config->buf_len);
        ion.bundle_size += config->buf_len;

        config->buf_len = 0;
    }

    if ((*bdm_bp_funcs.sdr_end_xn)(ion.sdr) < 0 || ion.bundleZco == 0)
    {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                "Can't end sdr transaction");
            (*bdm_bp_funcs.sdr_cancel_xn)(ion.sdr);
            ion.sdr = NULL;
            return -1;
    }

    if ( ion.bundle_size ) {
        int r;
        Object unused_new_bundle;

        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "Sending bundle with payload of %lu bytes",
            (unsigned long)ion.bundle_size);

        r = (*bdm_bp_funcs.bp_send)(
            ion.sap,
            BP_BLOCKING,
            config->sync_recipient, 
            NULL,                     // report-to EID
            config->bundle_lifetime,  // Lifetime in seconds
            BP_STD_PRIORITY,          // class of service
            SourceCustodyRequired,
            0,                        // reporting flags - all disabled
            0,                        // app-level ack requested - what's this doing in BP?!
            NULL,                     // extended CoS - not used when CoS is STD_PRIORITY as above
            ion.bundleZco,
            &unused_new_bundle        // handle to the bundle in the BA, we dont need it (wish we could pass in NULL here)
        );

        if (r < 1) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error sending bundle!");
            return -1;
        }
    } else {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "no bundle to send!");
        // FIXME: what's the right way to delete the bundle?
        // zco_destroy_reference(ion.sdr, ion.bundleZco);
    }


    (*bdm_bp_funcs.writeErrmsgMemos)();
    return 0;

}
#endif

static int write_data_to_message(const void *buffer, size_t size, void * config_void)
{
    int rc;
    sync_sender_config_t * config = (sync_sender_config_t *)config_void;
    switch ( config->method ) {
        case BDM_SYNC_METHOD_TCP:
            rc = write_data_to_socket(buffer, size, config);
            break;

#if ENABLE_ION
        case BDM_SYNC_METHOD_ION:
            rc = write_data_to_ion(buffer, size, config);
            break;
#endif
        
        default:
            g_log(
                BDM_LOG_DOMAIN,
                G_LOG_LEVEL_WARNING,
                "write_data_to_message(): failed to write: unknown method %d",
                config->method);
            rc = -1;
    }
    return rc;
}

static int sync_finish_connection(sync_sender_config_t * config) {
    int rc;
    switch ( config->method ) {
        case BDM_SYNC_METHOD_TCP:
            rc = sync_finish_connection_tcp(config);
            break;

#if ENABLE_ION
        case BDM_SYNC_METHOD_ION:
            rc = sync_finish_connection_ion(config);
            break;
#endif
        
        default:
            g_log(
                BDM_LOG_DOMAIN,
                G_LOG_LEVEL_WARNING,
                "sync_finish_connection(): failed to send: unknown method %d",
                config->method);
            rc = -1;
    }
    return rc;
}



static int sync_init_connection_tcp(sync_sender_config_t * config) {
    struct sockaddr_in server_address;
    struct hostent *server_host;

    if (NULL == config) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "sync_init_connection_tcp(): NULL config passed in.");
	return -1;
    }

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	  "    Initializing sync connection");

    // If the server dies or the connection is lost somehow, writes will
    // cause us to receive SIGPIPE, and the default SIGPIPE handler
    // terminates the process.  So we need to change the handler to ignore
    // the signal, unless the process has explicitly changed the action.
    {
        int r;
        struct sigaction sa;

        r = sigaction(SIGPIPE, NULL, &sa);
        if (r < 0) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "sync_init_connection_tcp(): error getting old SIGPIPE sigaction: %m");
            return -1;
        }

        if (sa.sa_handler == SIG_DFL) {
            sa.sa_handler = SIG_IGN;
            r = sigaction(SIGPIPE, &sa, NULL);
            if (r < 0) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "sync_init_connection_tcp(): error setting SIGPIPE sigaction to SIG_IGN: %m");
                return -1;
            }
        }
    }

    // get the hostent for the server
    server_host = gethostbyname(config->sync_recipient);
    if (server_host == NULL) {
	const char *error_string;
	error_string = hstrerror(h_errno);

        g_log(
            BDM_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "sync_init_connection_tcp(): gethostbyname(\"%s\"): %s",
            config->sync_recipient,
            error_string
        );
        return -1;
    }
    
        // create the socket
    if ((config->fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        g_log(
            BDM_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "sync_init_connection_tcp(): cannot create local socket: %m");
        return -1;
    }


    // prepare the server address
    memset((char *)&server_address, '\0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)(*server_host->h_addr_list)));
    server_address.sin_port = g_htons(config->remote_port);


    // connect to the server
    if (connect(config->fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        g_log(
            BDM_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "sync_init_connection_tcp(): failed to connect to server %s:%d: %s",
            server_host->h_name,
            config->remote_port,
            strerror(errno));
        close(config->fd);
        config->fd = -1;
        return -1;
    }

    config->bytes_sent = 0;

    return 0;
}

static int sync_init_connection(sync_sender_config_t * config) {
    int rc;
    switch ( config->method ) {
        case BDM_SYNC_METHOD_TCP:
            rc = sync_init_connection_tcp(config);
            break;

#if ENABLE_ION
        case BDM_SYNC_METHOD_ION:
            rc = sync_init_connection_ion(config);
            break;
#endif
        
        default:
            g_log(
                BDM_LOG_DOMAIN,
                G_LOG_LEVEL_WARNING,
                "sync_init_connection(): failed to initialize: unknown method %d",
                config->method);
            rc = -1;
    }
    return rc;
}



// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
