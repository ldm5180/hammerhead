
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#define _ISOC99_SOURCE //needed for strtof()
#include <stdlib.h>
#include <glib.h>

#include "libhab-internal.h"
#include "cal-server.h"
#include "bionet-asn.h"



static void libhab_set_resource(const char *peer_name, SetResourceValue_t *set_resource_value) {
    int ni;

    for (ni = 0; ni < bionet_hab_get_num_nodes(libhab_this); ni ++) {
        bionet_node_t *node;
        int ri;

        node = bionet_hab_get_node_by_index(libhab_this, ni);
        if (! bionet_node_matches_id(node, (char *)set_resource_value->nodeId.buf)) continue;

        for (ri = 0; ri < bionet_node_get_num_resources(node); ri ++) {
            bionet_resource_t *resource;
            bionet_datapoint_t *datapoint;

            resource = bionet_node_get_resource_by_index(node, ri);
            if (! bionet_resource_matches_id(resource, (char *)set_resource_value->resourceId.buf)) continue;

	    bionet_value_t * value;
	    switch (bionet_resource_get_data_type(resource))
	    {
	    case BIONET_RESOURCE_DATA_TYPE_BINARY:
	    {
		value = bionet_value_new_binary(resource, atoi((const char *)set_resource_value->value.buf));
		break;
	    }
	    case BIONET_RESOURCE_DATA_TYPE_UINT8:
	    {
		value = bionet_value_new_uint8(resource, 
					       (uint8_t)strtoul((const char *)set_resource_value->value.buf, NULL, 0));
		break;
	    }
	    case BIONET_RESOURCE_DATA_TYPE_INT8:
	    {
		value = bionet_value_new_int8(resource, 
					      (int8_t)atoi((const char *)set_resource_value->value.buf));
		break;
	    }
	    case BIONET_RESOURCE_DATA_TYPE_UINT16:
	    {
		value = bionet_value_new_uint16(resource, 
						(uint16_t)strtoul((const char *)set_resource_value->value.buf, NULL, 0));
		break;
	    }
	    case BIONET_RESOURCE_DATA_TYPE_INT16:
	    {
		value = bionet_value_new_int16(resource, 
					       (int16_t)atoi((const char *)set_resource_value->value.buf));
		break;
	    }
	    case BIONET_RESOURCE_DATA_TYPE_UINT32:
	    {
		value = bionet_value_new_uint32(resource, 
						(uint32_t)strtoul((const char *)set_resource_value->value.buf, NULL, 0));
		break;
	    }
	    case BIONET_RESOURCE_DATA_TYPE_INT32:
	    {
		value = bionet_value_new_int32(resource, 
					       (int32_t)atoi((const char *)set_resource_value->value.buf));
		break;
	    }
	    case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	    {
		value = bionet_value_new_float(resource, 
					       strtof((const char *)set_resource_value->value.buf, NULL));
		break;
	    }
	    case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	    {
		value = bionet_value_new_double(resource, 
						strtod((const char *)set_resource_value->value.buf, NULL));
		break;
	    }
	    case BIONET_RESOURCE_DATA_TYPE_STRING:
	    {
		value = bionet_value_new_str(resource, 
					     (char *)set_resource_value->value.buf);
		break;	
	    }
	    default:
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "libhab_set_resource(): Invalid datatype");
		break;
	    }
	    datapoint = bionet_datapoint_new(resource, value, NULL);

            if (datapoint == NULL) {
                g_log(
                    BIONET_LOG_DOMAIN,
                    G_LOG_LEVEL_WARNING,
                    "set-resource request from '%s' %s:%s = '%s' cannot parse value to data type %s of %s",
                    peer_name,
                    (char *)set_resource_value->nodeId.buf,
                    (char *)set_resource_value->resourceId.buf,
                    (char *)set_resource_value->value.buf,
                    bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)),
		    bionet_resource_get_local_name(resource)
                );
                continue;
            }

            libhab_callback_set_resource(resource, value);

            bionet_datapoint_free(datapoint);
        }
    }
}




static void libhab_handle_datapoint_subscription_request(const char *peer_name, const char *topic) {
    char topic_node_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char topic_resource_id[BIONET_NAME_COMPONENT_MAX_LEN];
    int r;

    r = bionet_split_nodeid_resourceid_r(&topic[2], topic_node_id, topic_resource_id);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "client '%s' requests invalid Datapoint subscription topic '%s'", peer_name, &topic[2]);
        return;
    }
    if (!bionet_is_valid_name_component_or_wildcard(topic_node_id)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "client '%s' requests invalid Datapoint subscription topic '%s'", peer_name, &topic[2]);
        return;
    }
    if (!bionet_is_valid_name_component_or_wildcard(topic_resource_id)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "client '%s' requests invalid Datapoint subscription topic '%s'", peer_name, &topic[2]);
        return;
    }

    // send all matching resource metadata and datapoints
    int i;
    for (i = 0; i < bionet_hab_get_num_nodes(libhab_this); i++) {
        bionet_node_t *node = bionet_hab_get_node_by_index(libhab_this, i);
        int ri;

        if (!bionet_name_component_matches(bionet_node_get_id(node), topic_node_id)) {
            continue;
        }

        for (ri = 0; ri < bionet_node_get_num_resources(node); ri ++) {
            bionet_resource_t *resource = bionet_node_get_resource_by_index(node, ri);
            bionet_asn_buffer_t buf;
            int r;

            if (!bionet_name_component_matches(bionet_resource_get_id(resource), topic_resource_id)) {
                continue;
            }


            //
            // first send the metadata for this matching resource
            //

            r = bionet_resource_metadata_to_asnbuf(resource, &buf);
            if (r != 0) {
                // an error has already been logged, and the buffer has been freed
                continue;
            }

            // "publish" the message to the newly connected subscriber (via sendto)
            // cal_server.sendto takes the buf so we dont need to free it
            cal_server.sendto(peer_name, buf.buf, buf.size);


            //
            // then send the datapoints (if there are any)
            //

            r = bionet_resource_datapoints_to_asnbuf(resource, &buf, 0);
            if (r != 0) {
                // an error has already been logged, and the buffer has been freed
                continue;
            }

            // "publish" the message to the newly connected subscriber (via sendto)
            // cal_server.sendto takes the buf so we dont need to free it
            cal_server.sendto(peer_name, buf.buf, buf.size);

        }
    }

    cal_server.subscribe(peer_name, topic);
}




static void libhab_handle_stream_subscription_request(const char *peer_name, const char *topic) {
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

    node = bionet_hab_get_node_by_id(libhab_this, topic_node_id);
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

    if (libhab_callback_stream_subscription != NULL) {
        libhab_callback_stream_subscription(peer_name, stream);
    }

    cal_server.subscribe(peer_name, topic);
}




static void libhab_handle_node_list_subscription_request(const char *peer_name, const char *topic) {

    if (!bionet_is_valid_name_component_or_wildcard(&topic[2])) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "client '%s' requests invalid Node-list subscription topic '%s'", peer_name, &topic[2]);
        return;
    }

    // send all matching nodes
    int i;
    for (i = 0; i < bionet_hab_get_num_nodes(libhab_this); i++) {
        bionet_asn_buffer_t buf;
        bionet_node_t *node = bionet_hab_get_node_by_index(libhab_this, i);
        int r;

        if (!bionet_name_component_matches(bionet_node_get_id(node), &topic[2])) continue;

        r = bionet_node_to_asnbuf(node, &buf);
        if (r != 0) {
            // an error has already been logged, and the buffer has been freed
            continue;
        }

        // "publish" the message to the newly connected subscriber (via sendto)
        // cal_server.sendto takes the buf so we dont need to free it
        cal_server.sendto(peer_name, buf.buf, buf.size);
    }

    cal_server.subscribe(peer_name, topic);
}


// FIXME: authn/authz the client's requested subscription topic and accept or reject
static void libhab_handle_subscription_request(const char *peer_name, const char *topic) {
    // stream subscription?
    if (strncmp(topic, "S ", 2) == 0) {
        libhab_handle_stream_subscription_request(peer_name, topic);
        return;
    }

    // datapoint subscription?
    if (strncmp(topic, "D ", 2) == 0) {
        libhab_handle_datapoint_subscription_request(peer_name, topic);
        return;
    }

    // node subscription then, hopefully
    if (strncmp(topic, "N ", 2) == 0) {
        libhab_handle_node_list_subscription_request(peer_name, topic);
        return;
    }
}




static void libhab_handle_stream_unsubscription_request(const char *peer_name, const char *topic) {
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

    node = bionet_hab_get_node_by_id(libhab_this, topic_node_id);
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

    if (libhab_callback_stream_unsubscription != NULL) {
        libhab_callback_stream_unsubscription(peer_name, stream);
    }
}




static void libhab_handle_unsubscription_request(const char *peer_name, const char *topic) {
    // stream unsubscription?
    if (strncmp(topic, "S ", 2) == 0) {
        libhab_handle_stream_unsubscription_request(peer_name, &topic[2]);
        return;
    }

    // datapoint unsubscription and node-list unsubscription are not handled yet
}




static void libhab_stream_data(const char *peer_name, StreamData_t *sd) {
    bionet_node_t *node;
    bionet_stream_t *stream;

    node = bionet_hab_get_node_by_id(libhab_this, (char *)sd->nodeId.buf);
    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "got a Stream Data message from Client '%s', for unknown Node %s", peer_name, sd->nodeId.buf);
        return;
    }

    stream = bionet_node_get_stream_by_id(node, (char *)sd->streamId.buf);
    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "got a Stream Data message from Client '%s', for unknown Stream %s:%s", peer_name, bionet_node_get_id(node), sd->streamId.buf);
        return;
    }

    libhab_callback_stream_data(peer_name, stream, (const char *)sd->data.buf, sd->data.size);
}




void libhab_cal_callback(const cal_event_t *event) {
    switch (event->type) {
        case CAL_EVENT_CONNECT: {
            // we don't do anything with this
            break;
        }

        case CAL_EVENT_DISCONNECT: {
            if (libhab_callback_lost_client != NULL) {
                libhab_callback_lost_client(event->peer_name);
            }
            break;
        }


        case CAL_EVENT_MESSAGE: {
            C2H_Message_t *m = NULL;
            asn_dec_rval_t rval;

            rval = ber_decode(NULL, &asn_DEF_C2H_Message, (void **)&m, event->msg.buffer, event->msg.size);
            if (rval.code == RC_WMORE) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "server message from '%s' contained an incomplete ASN.1 message", event->peer_name);
                asn_DEF_C2H_Message.free_struct(&asn_DEF_C2H_Message, m, 0);
                return;
            } else if (rval.code == RC_FAIL) {
                // received invalid junk
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "server message from '%s' contained an invalid ASN.1 message", event->peer_name);
                asn_DEF_C2H_Message.free_struct(&asn_DEF_C2H_Message, m, 0);
                return;
            } else if (rval.code != RC_OK) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "unknown error (code=%d) decoding server message from '%s'", rval.code, event->peer_name);
                asn_DEF_C2H_Message.free_struct(&asn_DEF_C2H_Message, m, 0);
                return;
            }

            if (rval.consumed != event->msg.size) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "server message from '%s' contained junk at end of message (consumed %d of %d)", event->peer_name, (int)rval.consumed, event->msg.size);
            }

            switch (m->present) {
                case C2H_Message_PR_setResourceValue: {
                    if (libhab_callback_set_resource != NULL) {
                        libhab_set_resource(event->peer_name, &m->choice.setResourceValue);
                    }
                    break;
                }

                case C2H_Message_PR_streamData: {
                    if (libhab_callback_stream_data != NULL) {
                        libhab_stream_data(event->peer_name, &m->choice.streamData);
                    }
                    break;
                }

                default: {
                    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Got unknown message %d from '%s'", m->present, event->peer_name);
                    break;
                }
            }

            asn_DEF_C2H_Message.free_struct(&asn_DEF_C2H_Message, m, 0);

            break;
        }


        case CAL_EVENT_SUBSCRIBE: {
            libhab_handle_subscription_request(event->peer_name, event->topic);
            break;
        }


        case CAL_EVENT_UNSUBSCRIBE: {
            // g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "%s unsubscribes '%s'", event->peer_name, event->topic);
            libhab_handle_unsubscription_request(event->peer_name, event->topic);
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
