
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>

#include "libhab-internal.h"


int libhab_cal_topic_matches(const char *topic, const char *subscription) {

    //
    // There are three kinds of subscriptions: node subscriptions, stream
    // subscriptions, and datapoint subscriptions.
    //
    // Node subscriptions are of the form "N <NodeId>"
    //
    // Stream subscriptions are of the form "S <NodeID:StreamID>"
    //
    // Datapoint subscriptions are of the form "D <NodeID:ResourceID>"
    //


    //
    // first of all, the family of the topic of the subscription and the
    // family of the topic of the published info need to be the same
    //

    if (strncmp(subscription, topic, 2) != 0) return -1;


    //
    // Node subscription
    //

    if (subscription[0] == 'N') {
        if (bionet_name_component_matches(&topic[2], &subscription[2])) {
            return 0;
        } else {
            return -1;
        }
    }


    //
    // Stream subscription
    //

    if (subscription[0] == 'S') {
        char sub_node_id[BIONET_NAME_COMPONENT_MAX_LEN];
        char sub_stream_id[BIONET_NAME_COMPONENT_MAX_LEN];

        char topic_node_id[BIONET_NAME_COMPONENT_MAX_LEN];
        char topic_stream_id[BIONET_NAME_COMPONENT_MAX_LEN];

        int r;

        r = bionet_split_nodeid_resourceid_r(&subscription[2], sub_node_id, sub_stream_id);
        if (r != 0) return -1;

        r = bionet_split_nodeid_resourceid_r(&topic[2], topic_node_id, topic_stream_id);
        if (r != 0) return -1;

        if (!bionet_name_component_matches(topic_node_id, sub_node_id)) return -1;
        if (!bionet_name_component_matches(topic_stream_id, sub_stream_id)) return -1;

        return 0;
    }


    //
    // If the subscription begins with "D ", then it's a Datapoint subscription
    //

    if (subscription[0] == 'D') {
        char sub_node_id[BIONET_NAME_COMPONENT_MAX_LEN];
        char sub_resource_id[BIONET_NAME_COMPONENT_MAX_LEN];

        char topic_node_id[BIONET_NAME_COMPONENT_MAX_LEN];
        char topic_resource_id[BIONET_NAME_COMPONENT_MAX_LEN];

        int r;

        r = bionet_split_nodeid_resourceid_r(&subscription[2], sub_node_id, sub_resource_id);
        if (r != 0) return -1;

        r = bionet_split_nodeid_resourceid_r(&topic[2], topic_node_id, topic_resource_id);
        if (r != 0) return -1;

        if (!bionet_name_component_matches(topic_node_id, sub_node_id)) return -1;
        if (!bionet_name_component_matches(topic_resource_id, sub_resource_id)) return -1;

        return 0;
    }


    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "unknown publish topic '%s'", topic);
    return -1;
}

