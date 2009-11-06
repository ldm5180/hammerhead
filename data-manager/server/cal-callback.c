
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#define _ISOC99_SOURCE //needed for strtof()
#define _SVID_SOURCE //needed for strdup
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "bionet-data-manager.h"
#include "cal-server.h"
#include "bdm-asn.h"
#include "subscription.h"

static bdm_peer_states_t * peer_states = NULL;

static void libbdm_process_subscription_request(
        const char *peer_name,
        const char *topic,
        bdm_subscription_t *state);

static void libbdm_publishto_each_resource(
        GPtrArray * bdm_list,
        int this_seq,
        const char * peer_name,
        int (*encode_resource)(bionet_bdm_t * bdm, bionet_resource_t *resource, 
                                long entry_seq, bionet_asn_buffer_t *buf) 
        ) 
{
    int bi, r;
    char datapoint_topic[4*BIONET_NAME_COMPONENT_MAX_LEN + 4];

    for (bi = 0; bi < bdm_list->len; bi++) {
	int hi;
	bionet_bdm_t * bdm = g_ptr_array_index(bdm_list, bi);
	if (NULL == bdm) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "Failed to get BDM %d from BDM list", bi);
            continue;
	}

	//walk list of habs
	for (hi = 0; hi < bionet_bdm_get_num_habs(bdm); hi++) {
	    int ni;
	    bionet_hab_t * hab = bionet_bdm_get_hab_by_index(bdm, hi);
	    if (NULL == hab) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
		      "Failed to get HAB %d from array of HABs", hi);
                continue;
	    }
	    
	    //walk list of nodes
	    for (ni = 0; ni < bionet_hab_get_num_nodes(hab); ni++) {
		int ri;
		bionet_node_t * node = bionet_hab_get_node_by_index(hab, ni);
		if (NULL == node) {
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
			  "Failed to get node %d from HAB %s", ni, bionet_hab_get_name(hab));
                    continue;
		}
		
		
		//walk list of resources
		for (ri = 0; ri < bionet_node_get_num_resources(node); ri++) {
                    bionet_asn_buffer_t buf;
		    bionet_resource_t * resource;
                   
                    resource = bionet_node_get_resource_by_index(node, ri);
		    if (NULL == resource) {
			g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
			      "Failed to get resource %d from Node %s",
                              ri, bionet_node_get_name(node));
                        continue;
		    }

                    snprintf(datapoint_topic, sizeof(datapoint_topic), "D %s/%s.%s", 
                        bionet_bdm_get_id(bdm), bionet_hab_get_name(hab), 
                        bionet_resource_get_local_name(resource));


                    //
                    // Encode the resource with the supplied function
                    r = encode_resource(bdm, resource, this_seq, &buf);
                    if (r != 0) {
                        // an error has already been logged, and the buffer has been freed
                        continue;
                    }

                    // "publish" the message to the newly connected subscriber (via publishto)
                    // if the datapoint topic does not match any previous topics
                    cal_server.publishto(peer_name, datapoint_topic, buf.buf, buf.size);

                    // FIXME: cal_server.publish should take the buf
                    free(buf.buf);

		} //for each resource
	    } //for each node
	} //for each hab
    } //for each bdm
}


//
// Handle the subscription request once both the actual subscription and state have been sent
//
//
// Construct and send publish messages with the results of a
// database query. This is inefficient for large changesets. 
//
// Send a sync record when thats the case (Which has the tradeoff of
// not removing duplicates from overlapping subscriptions...)
//
// @param query_results
//   The results of db_get_resource_datapoints()
//
// TODO: There are a few ways this is broken.
//  1 - Like BDMSync, there's no way on initial subscribe to filter metadata by 
//  datapoint time, becuase the database doesn't store naode/hab arrive/depart.
//
//  2 - Becuase the messages are grouped in sync records, there's no way to filter out
//  metadata that match multiple subscriptions. This is a result of the compromise detailed in #1
//  
//  3 - This could be done more efficiently by re-writing db_get_resource_datapoints()
//  to directly publish... That would limit the data in-memory at once.
//
static void libbdm_process_datapoint_subscription_request(
        const char *peer_name,
        const char *topic,
        bdm_subscription_t *state)
{
    char topic_hab_type[BIONET_NAME_COMPONENT_MAX_LEN];
    char topic_hab_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char topic_node_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char topic_resource_id[BIONET_NAME_COMPONENT_MAX_LEN];
    int r;
    GPtrArray *bdm_list;

    struct timeval tv_start;
    struct timeval *pDatapointStart = NULL;
    struct timeval tv_stop;
    struct timeval *pDatapointEnd = NULL;

    r = bionet_split_resource_name_r(&topic[2], topic_hab_type, topic_hab_id, topic_node_id, topic_resource_id);
    if (r < 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            "client '%s' requests invalid Datapoint subscription topic '%s'", peer_name, &topic[2]);
        return;
    }

    if ( r > 0 ) {
        GHashTable * params = NULL;
        r = bionet_parse_topic_params(topic + 2 + r, &params);
        if (r != 0) {
            // Error logged
            return;
        }

        if( 0 == bionet_param_to_timeval(params, "dpstart", &tv_start) ) {
            pDatapointStart = &tv_start;
        }
        if( 0 == bionet_param_to_timeval(params, "dpend", &tv_stop) ) {
            pDatapointEnd = &tv_stop;
        }
    }

    // do that database lookup
    long this_seq = db_get_latest_entry_seq(main_db);

    bdm_list = db_get_metadata(main_db,
        strcmp("*",topic_hab_type)?topic_hab_type:NULL,
        strcmp("*",topic_hab_id)?topic_hab_id:NULL,
        strcmp("*",topic_node_id)?topic_node_id:NULL,
        strcmp("*",topic_resource_id)?topic_resource_id:NULL,
        pDatapointStart, pDatapointEnd, 
        0, this_seq);
    if (NULL == bdm_list) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
	      "Failed to get a BDM list.");
	return;
    }
    libbdm_publishto_each_resource(bdm_list, this_seq, peer_name, 
            bdm_resource_metadata_to_asnbuf); 

    
    bdm_list = db_get_resource_datapoints(main_db,
        strcmp("*",topic_hab_type)?topic_hab_type:NULL,
        strcmp("*",topic_hab_id)?topic_hab_id:NULL,
        strcmp("*",topic_node_id)?topic_node_id:NULL,
        strcmp("*",topic_resource_id)?topic_resource_id:NULL,
        pDatapointStart, pDatapointEnd, 
        state->curr_seq, this_seq);
    if (NULL == bdm_list) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
	      "Failed to get a BDM list.");
	return;
    }

    libbdm_publishto_each_resource(bdm_list, this_seq, peer_name, 
            bdm_resource_datapoints_to_asnbuf); 

    // Tell CAL we'll accept this subscription
    cal_server.subscribe(peer_name, topic);
}

static void libbdm_handle_sent_state(
        const char * peer_name,
        BDMSendState_t *ss)
{
    if(NULL == peer_states){
        peer_states = bdm_subscriptions_new();
    }

    char * topic = (char*)ss->topic.buf;
    bdm_subscription_t * state =  bdm_subscriptions_get(peer_states, peer_name, topic);
    if (state == NULL ) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "client '%s' sends state for '%s' before subscription", peer_name, &topic[2]);
        return;
    }


    if(ss->seq < 0){
        state->curr_seq = 0;
    } else {
        state->curr_seq = ss->seq;
    }

    libbdm_process_subscription_request(peer_name, topic, state);
}


#if 0
static void libbdm_handle_stream_subscription_request(const char *peer_name, const char *topic) {
    char topic_node_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char topic_stream_id[BIONET_NAME_COMPONENT_MAX_LEN];
    bionet_node_t *node;
    bionet_stream_t *stream;
    int r;


    // 
    // sanity checks
    //

    r = bionet_split_nodeid_resourceid_r(&topic[2], topic_node_id, topic_stream_id);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "client '%s' requests invalid Stream subscription topic '%s'", peer_name, &topic[2]);
        return;
    }

    if (!bionet_is_valid_name_component(topic_node_id)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "client '%s' requests stream subscription with invalid Node '%s'", peer_name, topic_node_id);
        return;
    }

    if (!bionet_is_valid_name_component(topic_stream_id)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "client '%s' requests stream subscription with invalid Stream '%s'", peer_name, topic_stream_id);
        return;
    }


    //
    // the topic looks syntactically good, see if it matches an existing stream
    // NOTE: stream subscriptions differ from datapoint subscriptions in that you can only subscribe to existing streams
    //

    node = bionet_hab_get_node_by_id(libbdm_this, topic_node_id);
    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "client '%s' requests stream subscription with unknown Node '%s'", peer_name, topic_node_id);
        return;
    }

    stream = bionet_node_get_stream_by_id(node, topic_stream_id);
    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "client '%s' requests stream subscription with unknown Stream %s:%s", peer_name, bionet_node_get_id(node), topic_stream_id);
        return;
    }


    // 
    // ok it all looks good, report it to the HAB and accept the subscription
    //

    if (libbdm_callback_stream_subscription != NULL) {
        libbdm_callback_stream_subscription(peer_name, stream);
    }

    cal_server.subscribe(peer_name, topic);
}




static void libbdm_handle_node_list_subscription_request(const char *peer_name, const char *topic) {
    char node_topic[BIONET_NAME_COMPONENT_MAX_LEN + 2];

    if (!bionet_is_valid_name_component_or_wildcard(&topic[2])) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "client '%s' requests invalid Node-list subscription topic '%s'", peer_name, &topic[2]);
        return;
    }

    // send all matching nodes
    int i;
    for (i = 0; i < bionet_hab_get_num_nodes(libbdm_this); i++) {
        bionet_asn_buffer_t buf;
        bionet_node_t *node = bionet_hab_get_node_by_index(libbdm_this, i);
        int r;

        if (!bionet_name_component_matches(bionet_node_get_id(node), &topic[2])) continue;

        r = bionet_node_to_asnbuf(node, &buf);
        if (r != 0) {
            // an error has already been logged, and the buffer has been freed
            continue;
        }

        snprintf(node_topic, sizeof(node_topic), "N %s", bionet_node_get_id(node));

        // "publish" the message to the newly connected subscriber (via publishto)
        cal_server.publishto(peer_name, node_topic, buf.buf, buf.size);

        // FIXME: cal_server.publish should take the buf
        free(buf.buf);
    }

    cal_server.subscribe(peer_name, topic);
}
#endif

static void libbdm_handle_subscription_request(const char *peer_name, const char *topic) {
    if(NULL == peer_states){
        peer_states = bdm_subscriptions_new();
    }

    bdm_subscriptions_add_request(peer_states, peer_name, topic); 
}

// Process the subscription once the state has been sent
static void libbdm_process_subscription_request(
        const char *peer_name,
        const char *topic,
        bdm_subscription_t *state)
{
    // stream subscription?
    if (strncmp(topic, "S ", 2) == 0) {
        //libbdm_handle_stream_subscription_request(peer_name, topic);
        return;
    }

    // datapoint subscription?
    if (strncmp(topic, "D ", 2) == 0) {
        libbdm_process_datapoint_subscription_request(peer_name, topic, state);
        return;
    }

    // node subscription?
    if (strncmp(topic, "N ", 2) == 0) {
        //libbdm_handle_node_list_subscription_request(peer_name, topic);
        return;
    }

    // Hab subscription then, hopefully
    if (strncmp(topic, "H ", 2) == 0) {
        //libbdm_handle_hab_list_subscription_request(peer_name, topic);
        return;
    }
}



#if 0

static void libbdm_handle_stream_unsubscription_request(const char *peer_name, const char *topic) {
    char topic_node_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char topic_stream_id[BIONET_NAME_COMPONENT_MAX_LEN];
    bionet_node_t *node;
    bionet_stream_t *stream;
    int r;


    // 
    // sanity checks
    //

    r = bionet_split_nodeid_resourceid_r(topic, topic_node_id, topic_stream_id);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "client '%s' requests invalid stream unsubscription topic '%s'", peer_name, topic);
        return;
    }

    if (!bionet_is_valid_name_component(topic_node_id)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "client '%s' requests stream unsubscription with invalid Node '%s'", peer_name, topic_node_id);
        return;
    }

    if (!bionet_is_valid_name_component(topic_stream_id)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "client '%s' requests stream unsubscription with invalid Stream '%s'", peer_name, topic_stream_id);
        return;
    }


    //
    // the topic looks syntactically good, see if it matches an existing stream
    //

    node = bionet_hab_get_node_by_id(libbdm_this, topic_node_id);
    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "client '%s' requests stream unsubscription with unknown Node '%s'", peer_name, topic_node_id);
        return;
    }

    stream = bionet_node_get_stream_by_id(node, topic_stream_id);
    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "client '%s' requests stream unsubscription with unknown Stream %s:%s", peer_name, bionet_node_get_id(node), topic_stream_id);
        return;
    }


    // 
    // ok it all looks good, report it to the HAB
    //

    if (libbdm_callback_stream_unsubscription != NULL) {
        libbdm_callback_stream_unsubscription(peer_name, stream);
    }
}

#endif


static void libbdm_handle_unsubscription_request(const char *peer_name, const char *topic) {
    // stream unsubscription?
    if (strncmp(topic, "S ", 2) == 0) {
        //libbdm_handle_stream_unsubscription_request(peer_name, &topic[2]);
        return;
    }

    // datapoint unsubscription and node-list unsubscription are not handled yet
}


#if 0


static void libbdm_stream_data(const char *peer_name, StreamData_t *sd) {
    bionet_node_t *node;
    bionet_stream_t *stream;

    node = bionet_hab_get_node_by_id(libbdm_this, (char *)sd->nodeId.buf);
    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "got a Stream Data message from Client '%s', for unknown Node %s", peer_name, sd->nodeId.buf);
        return;
    }

    stream = bionet_node_get_stream_by_id(node, (char *)sd->streamId.buf);
    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "got a Stream Data message from Client '%s', for unknown Stream %s:%s", peer_name, bionet_node_get_id(node), sd->streamId.buf);
        return;
    }

    libbdm_callback_stream_data(peer_name, stream, (const char *)sd->data.buf, sd->data.size);
}

#endif



void libbdm_cal_callback(const cal_event_t *event) {
    switch (event->type) {
        case CAL_EVENT_CONNECT: {
            // we don't do anything with this
            break;
        }

        case CAL_EVENT_DISCONNECT: {
            bdm_subscriptions_remove_peer(peer_states, event->peer_name);
            break;
        }


        case CAL_EVENT_MESSAGE: {
            BDM_C2S_Message_t *m = NULL;
            asn_dec_rval_t rval;

            rval = ber_decode(NULL, &asn_DEF_BDM_C2S_Message, (void **)&m, event->msg.buffer, event->msg.size);
            if (rval.code == RC_WMORE) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                        "server message from '%s' contained an incomplete ASN.1 message",
                        event->peer_name);
                asn_DEF_BDM_C2S_Message.free_struct(&asn_DEF_BDM_C2S_Message, m, 0);
                return;
            } else if (rval.code == RC_FAIL) {
                // received invalid junk
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                    "server message from '%s' contained an invalid ASN.1 message",
                    event->peer_name);
                asn_DEF_BDM_C2S_Message.free_struct(&asn_DEF_BDM_C2S_Message, m, 0);
                return;
            } else if (rval.code != RC_OK) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                    "unknown error (code=%d) decoding server message from '%s'",
                    rval.code, event->peer_name);
                asn_DEF_BDM_C2S_Message.free_struct(&asn_DEF_BDM_C2S_Message, m, 0);
                return;
            }

            if (rval.consumed != event->msg.size) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                    "server message from '%s' contained junk at end of message (consumed %d of %d)",
                    event->peer_name, (int)rval.consumed, event->msg.size);
            }

            switch (m->present) {
                case BDM_C2S_Message_PR_resourceDatapointsQuery:
                    libbdm_handle_resourceDatapointsQuery(event->peer_name, 
                            &m->choice.resourceDatapointsQuery);
                    break;

                case BDM_C2S_Message_PR_sendState:
                    libbdm_handle_sent_state(event->peer_name, &m->choice.sendState);
                    break;

                default:
                    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                        "Got unknown message %d from '%s'",
                        m->present, event->peer_name);
                    break;
            }

            asn_DEF_BDM_C2S_Message.free_struct(&asn_DEF_BDM_C2S_Message, m, 0);

            break;
        }


        case CAL_EVENT_SUBSCRIBE: {
            libbdm_handle_subscription_request(event->peer_name, event->topic);
            break;
        }


        case CAL_EVENT_UNSUBSCRIBE: {
            // g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "%s unsubscribes '%s'", event->peer_name, event->topic);
            libbdm_handle_unsubscription_request(event->peer_name, event->topic);
            break;
        }


        default: {
            printf("unhandled CAL event type %d\n", event->type);
            break;
        }
    }
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
