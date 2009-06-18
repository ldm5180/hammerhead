
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

#if defined(LINUX) || defined(MACOSX)
    #include <netdb.h>
    #include <pwd.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
#endif

#include <bionet.h>
#include "bionet-util.h"
#include "bdm-util.h"
#include "bionet-data-manager.h"

int send_message_to_sync_receiver(const void *buffer, size_t size, void * config_void);

static int sync_send_metadata(sync_sender_config_t * config, struct timeval * last_sync) {
    GPtrArray * bdm_list = NULL;
    int curr_seq, last_entry_end_seq;
    char * hab_type;
    char * hab_id;
    char * node_id;
    char * resource_id;
    BDM_Sync_Message_t sync_message;
    BDM_Sync_Metadata_Message_t * message;
    int bi, r;

    //get the most recent entry timestamp in the database. use this as the entry 
    //end time for the query. this allows for the DB to act as the syncronization point
    //instead of creating our own locks.
    curr_seq = db_get_latest_entry_seq();

    if (bionet_split_resource_name(config->resource_name_pattern,
				   &hab_type, &hab_id, &node_id, &resource_id)) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "Failed to split resource name pattern: %s", config->resource_name_pattern);
    }


    bdm_list = db_get_metadata(hab_type, hab_id, node_id, resource_id,
			       &config->start_time, &config->end_time,
			       config->last_entry_end_seq, curr_seq,
			       &last_entry_end_seq);
    config->last_entry_end_seq = last_entry_end_seq;

    if (NULL == bdm_list) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "send_sync_metadata(): NULL BDM list from db_get_metadata()");
	goto cleanup;
    }


    memset(&message, 0x00, sizeof(BDM_Sync_Message_t));
    sync_message.present = BDM_Sync_Message_PR_metadataMessage;
    message = &sync_message.choice.metadataMessage;


    for (bi = 0; bi < bdm_list->len; bi++) {
	int hi;
	bdm_t * bdm = g_ptr_array_index(bdm_list, bi);
	if (NULL == bdm) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "Failed to get BDM %d from BDM list", bi);
	    goto cleanup;
	}

	GPtrArray * hab_list = bdm->hab_list;
	if (NULL == hab_list) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "sync_send_metadata(): Failed to get HAB list from array of BDMs");
	    goto cleanup;
	}

	for (hi = 0; hi < hab_list->len; hi++) {
	    int ni;
	    HardwareAbstractor_t * asn_hab;
	    bionet_hab_t * hab = g_ptr_array_index(hab_list, hi);
	    if (NULL == hab) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
		      "sync_send_metadata(): Failed to get HAB %d from array of HABs", hi);
	    }
	    
	    //add the HAB to the message
	    asn_hab = (HardwareAbstractor_t *)calloc(1, sizeof(HardwareAbstractor_t));
	    if (asn_hab == NULL) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "sync_send_metadata(): out of memory!");
		goto cleanup;
	    }
	    
	    r = asn_sequence_add(&message->list, asn_hab);
	    if (r != 0) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "sync_send_metadata(): error adding HAB to Sync Metadata: %s", strerror(errno));
		goto cleanup;
	    }
	    
	    r = OCTET_STRING_fromString(&asn_hab->type, bionet_hab_get_type(hab));
	    if (r != 0) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "sync_send_metadata(): error making OCTET_STRING for HAB-Type %s", bionet_hab_get_type(hab));
		goto cleanup;
	    }
	    
	    r = OCTET_STRING_fromString(&asn_hab->id, bionet_hab_get_id(hab));
	    if (r != 0) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "sync_send_metadata(): error making OCTET_STRING for HAB-ID %s", bionet_hab_get_id(hab));
		goto cleanup;
	    }
	    
	    for (ni = 0; ni < bionet_hab_get_num_nodes(hab); ni++) {
		int ri;
		Node_t * asn_node;
		bionet_node_t * node = bionet_hab_get_node_by_index(hab, ni);
		if (NULL == node) {
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
			  "sync_send_metadata(): Failed to get node %d from HAB %s", ni, bionet_hab_get_name(hab));
		}
		
		//add the Node to the message
		asn_node = (Node_t *)calloc(1, sizeof(Node_t));
		if (asn_node == NULL) {
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "sync_send_metadata(): out of memory!");
		    goto cleanup;
		}
		
		r = asn_sequence_add(&asn_hab->nodes.list, asn_node);
		if (r != 0) {
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "sync_send_metadata(): error adding Node to Sync Metadata: %m");
		    goto cleanup;
		}
		
		r = OCTET_STRING_fromString(&asn_node->id, bionet_node_get_id(node));
		if (r != 0) {
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
			  "sync_send_metadata(): error making OCTET_STRING for Node-ID %s", bionet_node_get_id(node));
		    goto cleanup;
		}
		
		for (ri = 0; ri < bionet_node_get_num_resources(node); ri++) {
		    Resource_t * asn_resource;
		    bionet_resource_t * resource = bionet_node_get_resource_by_index(node, ri);
		    if (NULL == resource) {
			g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
			      "sync_send_metadata(): Failed to get resource %d from Node %s", ri, bionet_node_get_name(node));
		    }
		    
		    //add the Resource to the message
		    asn_resource = (Resource_t *)calloc(1, sizeof(Resource_t));
		    if (asn_resource == NULL) {
			g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "sync_send_metadata(): out of memory!");
			goto cleanup;
		    }
		    
		    r = asn_sequence_add(&asn_node->resources.list, asn_resource);
		    if (r != 0) {
			g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
			      "sync_send_metadata(): error adding Resource to Sync Metadata: %m");
			goto cleanup;
		    }
		    
		    r = OCTET_STRING_fromString(&asn_resource->id, bionet_resource_get_id(resource));
		    if (r != 0) {
			g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
			      "sync_send_metadata(): error making OCTET_STRING for Resource-ID %s", bionet_resource_get_id(resource));
			goto cleanup;
		    }
		    
		    asn_resource->flavor = bionet_flavor_to_asn(bionet_resource_get_flavor(resource));
		    if (asn_resource->flavor == -1) {
			g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
			      "sync_send_metadata(): resource has invalid flavor");
			goto cleanup;
		    }
		    
		    asn_resource->datatype = bionet_datatype_to_asn(bionet_resource_get_data_type(resource));
		    if (asn_resource->datatype == -1) {
			g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
			      "sync_send_metadata(): resource has invalid datatype");
			goto cleanup;
		    }
		} //for (ri = 0; ri < bionet_node_get_num_resources(node); ri++)
	    } //for (ni = 0; ni < bionet_hab_get_num_nodes(hab); ni++)
	} //for (hi = 0; hi < hab_list->len; hi++)
    } //for (bi = 0; bi < bdm_list->len; bi++)


    // send the reply to the client
    asn_enc_rval_t asn_r;
    asn_r = der_encode(&asn_DEF_BDM_Sync_Message, &sync_message, send_message_to_sync_receiver, config);
    if (asn_r.encoded == -1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "send_sync_datapoints(): error with der_encode(): %m");
    }

    return 0;

cleanup:
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_Sync_Message, &message);
    return -1;
} /* sync_send_metadata() */

#if 0
static int sync_send_datapoints(sync_sender_config_t * config, struct timeval * last_sync) {
    GPtrArray * bdm_list = NULL;
    int curr_seq, last_entry_end_seq;
    char * hab_type;
    char * hab_id;
    char * node_id;
    char * resource_id;
    BDM_Sync_Message_t sync_message;
    BDM_Sync_Datapoints_Message_t * message;
    int r, bi;

    //get the most recent entry timestamp in the database. use this as the entry 
    //end time for the query. this allows for the DB to act as the syncronization point
    //instead of creating our own locks.
    curr_seq = db_get_latest_entry_seq();

    if (bionet_split_resource_name(config->resource_name_pattern,
				   &hab_type, &hab_id, &node_id, &resource_id)) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "Failed to split resource name pattern: %s", config->resource_name_pattern);
    }


    bdm_list = db_get_resource_datapoints(hab_type, hab_id, node_id, resource_id,
					  &config->start_time, &config->end_time,
					  config->last_entry_end_seq, curr_seq,
					  &last_entry_end_seq);
    config->last_entry_end_seq = last_entry_end_seq;

    if (NULL == bdm_list) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "send_sync_datapoints(): NULL BDM list from db_get_resource_datapoints()");
	goto cleanup;
    }

    memset(&message, 0x00, sizeof(BDM_Sync_Datapoints_Message_t));
    sync_message.present = BDM_Sync_Message_PR_datapointsMessage;
    message = &sync_message.choice.datapointsMessage;

    //create a sync record for each BDM
    for (bi = 0; bi < bdm_list->len; bi++) {
	int hi;
	bdm_t * bdm = g_ptr_array_index(bdm_list, bi);
	if (NULL == bdm) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "Failed to get BDM %d from BDM list", bi);
	    goto cleanup;
	}

	GPtrArray * hab_list = bdm->hab_list;
	if (NULL == hab_list) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "sync_send_metadata(): Failed to get HAB list from array of BDMs");
	    goto cleanup;
	}

	BDMSyncRecord_t * sync_record = (BDMSyncRecord_t *)calloc(1, sizeof(BDMSyncRecord_t));
	if (sync_record) {
	    r = asn_sequence_add(&message->list, sync_record);
	    if (r != 0) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
		      "sync_send_datapoints(): error adding sync record to BDM Sync Datapoints Message: %m");
	    }
	} else {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
		  "sync_send_datapoints(): Failed to malloc sync_record: %m");
	}
    
    
	//add BDM-ID to BDM Sync Record, not just a NULL
	r = OCTET_STRING_fromString(&sync_record->bdmID, bdm->bdm_id);
	if (r != 0) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "sync_send_datapoints(): Failed to set BDM ID");
	}
    
    
	//walk list of habs
	for (hi = 0; hi < hab_list->len; hi++) {
	    int ni;
	    bionet_hab_t * hab = g_ptr_array_index(hab_list, hi);
	    if (NULL == hab) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
		      "Failed to get HAB %d from array of HABs", hi);
	    }
	    
	    //walk list of nodes
	    for (ni = 0; ni < bionet_hab_get_num_nodes(hab); ni++) {
		int ri;
		bionet_node_t * node = bionet_hab_get_node_by_index(hab, ni);
		if (NULL == node) {
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
			  "Failed to get node %d from HAB %s", ni, bionet_hab_get_name(hab));
		}
		
		
		//walk list of resources
		for (ri = 0; ri < bionet_node_get_num_resources(node); ri++) {
		    int di;
		    ResourceRecord_t resource_rec;
		    bionet_resource_t * resource = bionet_node_get_resource_by_index(node, ri);
		    if (NULL == resource) {
			g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
			      "Failed to get resource %d from Node %s", ri, bionet_node_get_name(node));
		    }
		    
		    r = asn_sequence_add(&sync_record->syncResources, &resource_rec);
		    if (r != 0) {
			g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
			      "sync_send_datapoints(): Failed to add resource record.");
		    }
		    
		    r = OCTET_STRING_fromString(&resource_rec.resourceKey, NULL);
		    if (r != 0) {
			g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
			      "sync_send_datapoints(): Failed to set resource key");
		    }
		    
		    
		    //walk list of datapoints and add each one to the message
		    for (di = 0; di < bionet_resource_get_num_datapoints(resource); di++) {
			bionet_datapoint_t * d = bionet_resource_get_datapoint_by_index(resource, di);
			Datapoint_t *asn_datapoint;
			if (NULL == d) {
			    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
				  "Failed to get datapoint %d from Resource %s", di, bionet_resource_get_name(resource));
			}
			
			asn_datapoint = bionet_datapoint_to_asn(d);
			if (asn_datapoint == NULL) {
			    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
				  "send_sync_datapoints(): out of memory!");
			}
			
			r = asn_sequence_add(&resource_rec.resourceDatapoints.list, asn_datapoint);
			if (r != 0) {
			    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
				  "send_sync_datapoints(): error adding Datapoint to Resource: %m");
			}
		    } //for (di = 0; di < bionet_resource_get_num_datapoints(resource); di++) 
		} //for (ri = 0; ri < bionet_node_get_num_resources(node); ri++)
	    } //for (ni = 0; ni < bionet_hab_get_num_nodes(hab); ni++)
	} //for (hi = 0; hi < hab_list->len; hi++)
    } //for (bi = 0; bi < bdm_list->len; bi++)

    // send the reply to the client
    asn_enc_rval_t asn_r;
    asn_r = der_encode(&asn_DEF_BDM_Sync_Message, &sync_message, send_message_to_sync_receiver, config);
    if (asn_r.encoded == -1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "send_sync_datapoints(): error with der_encode(): %m");
    }

    return 0;


cleanup:
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_Sync_Message, &sync_message);
    return -1;
} /* sync_send_datapoints() */
#endif

int send_message_to_sync_receiver(const void *buffer, size_t size, void * config_void) {
    sync_sender_config_t * config = (sync_sender_config_t *)config_void;
    struct sockaddr_in server_address;
    struct hostent *server_host;
    int fd;
    int r;

    if (NULL == config) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "send_message_to_sync_receiver(): NULL config passed in.");
	return -1;
    }

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
		  "send_message_to_sync_receiver(): error getting old SIGPIPE sigaction: %m");
            return -1;
        }

        if (sa.sa_handler == SIG_DFL) {
            sa.sa_handler = SIG_IGN;
            r = sigaction(SIGPIPE, &sa, NULL);
            if (r < 0) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "send_message_to_sync_receiver(): error setting SIGPIPE sigaction to SIG_IGN: %m");
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
            "send_message_to_sync_receiver(): gethostbyname(\"%s\"): %s",
            config->sync_recipient,
            error_string
        );
        return -1;
    }
    
        // create the socket
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        g_log(
            BDM_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "send_message_to_sync_receiver(): cannot create local socket: %m");
        return -1;
    }


    //  This makes it to the underlying networking code tries to send any
    //  buffered data, even after we've closed the socket.
    {
        struct linger linger;

        linger.l_onoff = 1;
        linger.l_linger = 60;
        if (setsockopt(fd, SOL_SOCKET, SO_LINGER, (char *)&linger, sizeof(linger)) != 0) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "send_message_to_sync_reciever(): WARNING: cannot make socket linger: %m");
        }
    }


    // prepare the server address
    memset((char *)&server_address, '\0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)(*server_host->h_addr_list)));
    server_address.sin_port = g_htons(BDM_SYNC_PORT);


    // connect to the server
    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        g_log(
            BDM_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "send_message_to_sync_receiver(): failed to connect to server %s:%d: %m",
            server_host->h_name,
            BDM_SYNC_PORT);
        return -1;
    }

    r = write(fd, buffer, size);

    close(fd);
    return r;
} /* send_message_to_sync_receiver() */


gpointer sync_thread(gpointer config) {
    struct timeval last_sync = { 0, 0 };
    sync_sender_config_t * cfg = (sync_sender_config_t *)config;

    if (NULL == cfg) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
	      "sync_thread(): NULL config"); 
    }

    while (1) {
	sync_send_metadata(cfg, &last_sync);
//	sync_send_datapoints(cfg, &last_sync);
	
	g_usleep(cfg->frequency * G_USEC_PER_SEC);
    }

    return NULL;
} /* sync_thread() */



// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
