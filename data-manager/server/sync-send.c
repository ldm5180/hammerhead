
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

#include "bdm-stats.h"

#define _Min(x,y) ((x)<(y)?(x):(y))

#if ENABLE_ION
#include "bps/bps_socket.h"
#endif
char * dtn_endpoint_id = NULL;

static int sync_init_connection(sync_sender_config_t * config);
static void sync_cancel_connection(sync_sender_config_t * config);
static int sync_finish_connection(sync_sender_config_t * config);
static int write_data_to_message(const void *buffer, size_t size, void * config_void);
static int write_data_to_socket(const void *buffer, size_t size, void * config_void);
static gboolean sync_check(gpointer data);

#if ENABLE_ION
static int write_data_to_ion(const void *buffer, size_t size, void * config_void);
#endif

static int count_sync_events(BDM_Sync_Metadata_Message_t * msg) {

    int n = 0;

    int bi, hi, ni;
    for(bi=0; bi<msg->list.count; bi++) {
        DataManager_t * bdm = msg->list.array[bi]; 

        for(hi=0; hi<bdm->hablist.list.count; hi++) {
            BDM_HardwareAbstractor_t * hab = bdm->hablist.list.array[hi];
            n += hab->events.list.count;

            for(ni=0; ni<hab->nodes.list.count; ni++) {
                BDM_Node_t * node = hab->nodes.list.array[ni];
                n += node->events.list.count;
            }
        }
    }

    return n;
}

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
    int r = -1;

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	  "Syncing metadata to %s", config->sync_recipient);


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

    bdm_sync_metadata_to_asn_setup(bdm_list, config->sync_mtu, config->db_key, from_seq, to_seq, &make_message_state, &iter);

    while((sync_message = bdm_sync_metadata_to_asn(&iter, &make_message_state)))
    {
        int num_events = count_sync_events(&sync_message->data.choice.metadataMessage);
        if ( 0 == num_events ) {
            continue;
        }

        if (sync_init_connection(config)) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
                  "    Unable to setup connection to BDM Sync Receiver");
            bdm_list_free(bdm_list);
            return -1;
        }

	// send the reply to the client
	asn_enc_rval_t asn_r;
	asn_r = der_encode(&asn_DEF_BDM_Sync_Message, sync_message, 
            write_data_to_message, config);


        ASN_STRUCT_FREE(asn_DEF_BDM_Sync_Message, sync_message);

	if (asn_r.encoded == -1) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                    "send_sync_metadata(): error with der_encode(%s)", 
                    asn_r.failed_type ? asn_r.failed_type->name : "unknown");
            bdm_list_free(bdm_list);
            sync_cancel_connection(config);
            return -1;
	}

        r = sync_finish_connection(config);
        if ( r == 0 ) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
                  "    Sync metadata finished");

            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
                  "Syncd %d metadata events to %s for seqs [%d,%d]",
                  num_events,
                  config->sync_recipient,
                  from_seq, to_seq);
            num_sync_sent_events += num_events;

            num_syncs_sent++;

            db_record_sync(config, from_seq, to_seq, 0);
        } else {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
                  "    Sync metadata FAILED");

        }
    }

    bdm_list_free(bdm_list);

    return r;

} /* sync_send_metadata() */


static int count_sync_datapoints(BDM_Sync_Message_t * msg) {

    int n = 0;

    int i;
    for(i=0; i<msg->data.choice.datapointsMessage.list.count; i++) {
        int j;
        BDMSyncRecord_t * syncRecord = msg->data.choice.datapointsMessage.list.array[i]; 

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

    bdm_sync_datapoints_to_asn_setup(bdm_list, config->sync_mtu, config->db_key, from_seq, to_seq, &make_message_state, &iter);

    while((sync_message = bdm_sync_datapoints_to_asn(&iter, &make_message_state)))
    {
        int num_datapoints = count_sync_datapoints(sync_message);
    
        if( 0 == num_datapoints ) {
            continue;
        }

        if (sync_init_connection(config)) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
                  "    Unable to make TCP connection to BDM Sync Receiver");
            bdm_list_free(bdm_list);
            return -1;
        }

	// send the reply to the client
	asn_enc_rval_t asn_r;
	asn_r = der_encode(&asn_DEF_BDM_Sync_Message, sync_message, 
            write_data_to_message, config);


	if (asn_r.encoded == -1) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                    "send_sync_datapoints(): error with der_encode(%s)",
                    asn_r.failed_type ? asn_r.failed_type->name : "unknown");
            bdm_list_free(bdm_list);
            ASN_STRUCT_FREE(asn_DEF_BDM_Sync_Message, sync_message);
            goto cleanup;
	}

        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
              "Syncd %d datapoints to %s for seqs [%d,%d]",
              num_datapoints,
              config->sync_recipient,
              from_seq, to_seq);
        ASN_STRUCT_FREE(asn_DEF_BDM_Sync_Message, sync_message);
        num_sync_sent_events += num_datapoints;

        r = sync_finish_connection(config);
        if( r != 0 ) {
            bdm_list_free(bdm_list);
            goto cleanup;
        }
        num_syncs_sent++;
        db_record_sync(config, from_seq, to_seq, 1);
    }
    bdm_list_free(bdm_list);


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

    if(size == 0) {
        return 0;
    }

    if(config->sync_mtu > 0 && config->buf_len + config->bytes_sent + size > config->sync_mtu) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
            "Bundle would exceed MTU of %d (%zd+%zd)", 
            config->sync_mtu, config->buf_len + config->bytes_sent, size);
        return -1;
    }

    int buffer_index = 0;
    while(size) {
        int bytes_to_copy = _Min(size,BP_SEND_BUF_SIZE - config->buf_len);
        char * dst = config->send_buf + config->buf_len;
        memcpy(dst, buffer+buffer_index, bytes_to_copy);
        config->buf_len += bytes_to_copy;
        size -= bytes_to_copy;
        buffer_index += bytes_to_copy;

        if(config->buf_len >= BP_SEND_BUF_SIZE) {
            ssize_t bytes = bps_sendto(config->bp_fd, (void*)config->send_buf, BP_SEND_BUF_SIZE, MSG_MORE,
                &config->bp_dstaddr, sizeof(struct bps_sockaddr));
            if ( bytes != BP_SEND_BUF_SIZE ) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                    "Error writing to bundle: %s", strerror(errno));
                return -1;
            }
            config->bytes_sent += BP_SEND_BUF_SIZE;
            config->buf_len = 0;
        }
    }

    return 1;
}
#endif


GSource * sync_send_new_gio_source(sync_sender_config_t* cfg, const char * database_file, int bp_fd) {
    GSource * source = NULL;

    cfg->db = db_init(database_file);
    if(cfg->db == NULL) {
        return NULL;
    }


    if(cfg->frequency <= 0 ) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "Sync-send frequency is 0");
        return NULL;
    }

#if ENABLE_ION
    if(cfg->method == BDM_SYNC_METHOD_ION) {
        cfg->bp_fd = bp_fd;
    }
#endif

    cfg->last_metadata_sync = -1;
    cfg->last_datapoint_sync = -1;


    if(db_sync_sender_setup(cfg)) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
              "Failed to reconcile Config with database");
    }

    source = g_timeout_source_new_seconds(cfg->frequency);
    g_source_set_callback(source, sync_check, cfg, NULL);

    return source;
}


static gboolean sync_check(gpointer data) {
    sync_sender_config_t * cfg = (sync_sender_config_t *)data;
    int curr_seq = 0;


    curr_seq = db_get_latest_entry_seq(cfg->db);
    
    if (curr_seq > cfg->last_metadata_sync) {
        int start_seq = cfg->last_metadata_sync + 1;
        int r;

        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Syncing Metadata to %s seq [%d,%d]", 
                cfg->sync_recipient, start_seq, curr_seq);
	r = sync_send_metadata(cfg, start_seq, curr_seq);
        if ( r ) return TRUE;

        cfg->last_metadata_sync = curr_seq;
    }
	
    if (curr_seq > cfg->last_datapoint_sync) {
        int start_seq = cfg->last_datapoint_sync + 1;
        int r;

        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Syncing Datapoints to %s seq [%d,%d]", 
                cfg->sync_recipient, start_seq, curr_seq);
	r = sync_send_datapoints(cfg, start_seq, curr_seq);
        if ( r ) return TRUE;

        cfg->last_datapoint_sync = curr_seq;
    }

    return TRUE;
} /* sync_check() */


static void sync_cancel_connection(sync_sender_config_t *config) {
    switch ( config->method ) {
        case BDM_SYNC_METHOD_TCP:
            // nothing to do here
            break;

#if ENABLE_ION
        case BDM_SYNC_METHOD_ION:
            // nothing to do here
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
    BDM_Sync_Message_t sync_msg;
    BDM_Sync_Message_t * p_sync_msg = &sync_msg;

    memset(&sync_msg, 0, sizeof(sync_msg));

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
			  &asn_DEF_BDM_Sync_Message, 
			  (void **)&p_sync_msg, 
			  buffer, 
			  index);

        if (rval.code == RC_OK) {
            int rc = -1;
            switch(sync_msg.data.present) {
                case BDM_Sync_Data_PR_ackMetadata:
                    rc = handle_sync_metadata_ack_message(
                            sync_msg.data.choice.ackMetadata,
                            config->db_key,
                            sync_msg.firstSeq, sync_msg.lastSeq);
                    break;

                case BDM_Sync_Data_PR_ackDatapoints:
                    rc = handle_sync_datapoints_ack_message(
                            sync_msg.data.choice.ackDatapoints,
                            config->db_key,
                            sync_msg.firstSeq, sync_msg.lastSeq);
                    break;

                default:
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                            "Unexpected Sync Message type");
                    break;
            }

            return rc;

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
    ssize_t bytes;

    if(config->buf_len > 0) {
        bytes = bps_sendto(config->bp_fd, config->send_buf, config->buf_len, 0, 
                &config->bp_dstaddr, sizeof(struct bps_sockaddr));
        if ( bytes != config->buf_len ) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                "Error writing to bundle: %s", strerror(errno));
            config->bytes_sent = 0;
            return -1;
        }
        config->bytes_sent += config->buf_len;
        config->buf_len = 0;
    } else {
        // Send 0 bytes, without MSG_MORE to flush any queued data
        bytes = bps_sendto(config->bp_fd, NULL, 0, 0,
                &config->bp_dstaddr, sizeof(struct bps_sockaddr));
        if ( bytes != 0 ) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                "Error writing to bundle: %s", strerror(errno));
            config->bytes_sent = 0;
            return -1;
        }
    }

    config->bytes_sent = 0;

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
            config->bytes_sent = 0;
            rc = 0;
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
