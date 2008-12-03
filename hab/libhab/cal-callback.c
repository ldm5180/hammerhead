
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


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

            datapoint = bionet_datapoint_new_with_valuestr(resource, (char *)set_resource_value->value.buf, NULL);
            if (datapoint == NULL) {
                g_log(
                    BIONET_LOG_DOMAIN,
                    G_LOG_LEVEL_WARNING,
                    "set-resource request from '%s' %s:%s = '%s' cannot parse value to data type %s of %s:%s",
                    peer_name,
                    (char *)set_resource_value->nodeId.buf,
                    (char *)set_resource_value->resourceId.buf,
                    (char *)set_resource_value->value.buf,
                    bionet_resource_data_type_to_string(resource->data_type),
                    node->id,
                    resource->id
                );
                continue;
            }

            libhab_callback_set_resource(resource, &datapoint->value);

            bionet_datapoint_free(datapoint);
        }
    }
}


void libhab_cal_callback(const cal_event_t *event) {
    switch (event->type) {
        case CAL_EVENT_CONNECT: {
            // we don't do anything with this
            break;
        }

        case CAL_EVENT_DISCONNECT: {
            // we don't do anything with this
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

            if (m->present != C2H_Message_PR_setResourceValue) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Got unknown message %d from '%s'", m->present, event->peer_name);
                asn_DEF_C2H_Message.free_struct(&asn_DEF_C2H_Message, m, 0);
                break;
            }

            if (libhab_callback_set_resource != NULL) {
                libhab_set_resource(event->peer_name, &m->choice.setResourceValue);
            }

            asn_DEF_C2H_Message.free_struct(&asn_DEF_C2H_Message, m, 0);

            break;
        }


        case CAL_EVENT_SUBSCRIBE: {
            GSList *i;

            // FIXME: authn/authz the client's requested subscription topic and accept or reject

            if (strchr(event->topic, ':') == NULL) {
                // node subscription
                if (!bionet_is_valid_name_component_or_wildcard(event->topic)) {
                    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "client '%s' requests invalid subscription topic '%s'", event->peer_name, event->topic);
                    break;
                }

                // send all matching nodes
                for (i = libhab_this->nodes; i != NULL; i = i->next) {
                    bionet_asn_buffer_t buf;
                    bionet_node_t *node = i->data;
                    int r;

                    if (libhab_cal_topic_matches(node->id, event->topic) != 0) continue;

                    r = bionet_node_to_asnbuf(node, &buf);
                    if (r != 0) {
                        // an error has already been logged, and the buffer has been freed
                        continue;
                    }

                    // "publish" the message to the newly connected subscriber (via sendto)
                    // cal_server.sendto takes the buf so we dont need to free it
                    cal_server.sendto(event->peer_name, buf.buf, buf.size);
                }

            } else {
                // datapoint subscription
                char topic_node_id[BIONET_NAME_COMPONENT_MAX_LEN];
                char topic_resource_id[BIONET_NAME_COMPONENT_MAX_LEN];
                int r;

                r = bionet_split_nodeid_resourceid_r(event->topic, topic_node_id, topic_resource_id);
                if (r != 0) {
                    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "client '%s' requests invalid datapoint subscription topic '%s'", event->peer_name, event->topic);
                    break;
                }
                if (!bionet_is_valid_name_component_or_wildcard(topic_node_id)) {
                    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "client '%s' requests invalid datapoint subscription topic '%s'", event->peer_name, event->topic);
                    break;
                }
                if (!bionet_is_valid_name_component_or_wildcard(topic_resource_id)) {
                    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "client '%s' requests invalid datapoint subscription topic '%s'", event->peer_name, event->topic);
                    break;
                }

                // send all matching resource metadata and datapoints
                for (i = libhab_this->nodes; i != NULL; i = i->next) {
                    bionet_node_t *node = i->data;
                    int ri;

                    if (!bionet_name_component_matches(node->id, topic_node_id)) {
                        continue;
                    }

                    for (ri = 0; ri < bionet_node_get_num_resources(node); ri ++) {
                        bionet_resource_t *resource = bionet_node_get_resource_by_index(node, ri);
                        bionet_asn_buffer_t buf;
                        int r;

                        if (!bionet_name_component_matches(resource->id, topic_resource_id)) {
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
                        cal_server.sendto(event->peer_name, buf.buf, buf.size);


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
                        cal_server.sendto(event->peer_name, buf.buf, buf.size);

                    }
                }
            }

            cal_server.subscribe(event->peer_name, event->topic);

            break;
        }

        default: {
            printf("unhandled CAL event type %d\n", event->type);
            break;
        }
    }
}

