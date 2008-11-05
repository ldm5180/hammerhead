
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <glib.h>

#include "libhab-internal.h"
#include "cal-server.h"
#include "bionet-asn.h"


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
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Got unexpected CAL Message event from '%s'", event->peer_name);
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

            break;
        }

        default: {
            printf("unhandled CAL event type %d\n", event->type);
            break;
        }
    }
}

