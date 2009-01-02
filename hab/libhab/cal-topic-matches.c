
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <string.h>

#include "libhab-internal.h"


int libhab_cal_topic_matches(const char *topic, const char *subscription) {

    //
    // There are three kinds of subscriptions: node subscriptions, stream
    // subscriptions, and datapoint subscriptions.
    //
    // Node subscriptions are of the form "<NodeId>"
    //
    // Stream subscriptions are of the form "S <NodeID:StreamID>"
    //
    // Datapoint subscriptions are of the form "<NodeID:ResourceID>"
    //


    //
    // If the subscription doesnt have a ':', then it's a Node subscription
    //

    if (strchr(subscription, ':') == NULL) {
        // this is a Node subscription
        if (strchr(topic, ':') == NULL) {
            // and a Node topic
            if (bionet_name_component_matches(topic, subscription)) return 0;
        }
        return -1;
    }


    //
    // If the subscription begins with "S ", then it's a Stream subscription
    //

    if (strncmp(subscription, "S ", 2) == 0) {
        char sub_node_id[BIONET_NAME_COMPONENT_MAX_LEN];
        char sub_stream_id[BIONET_NAME_COMPONENT_MAX_LEN];

        char topic_node_id[BIONET_NAME_COMPONENT_MAX_LEN];
        char topic_stream_id[BIONET_NAME_COMPONENT_MAX_LEN];

        int r;

        if (strncmp(topic, "S ", 2) != 0) {
            // not a Stream topic
            return -1;
        }

        r = bionet_split_nodeid_resourceid_r(subscription, sub_node_id, sub_stream_id);
        if (r != 0) return -1;

        r = bionet_split_nodeid_resourceid_r(topic, topic_node_id, topic_stream_id);
        if (r != 0) return -1;

        if (!bionet_name_component_matches(topic_node_id, sub_node_id)) return -1;
        if (!bionet_name_component_matches(topic_stream_id, sub_stream_id)) return -1;

        return 0;
    }


    //
    // this must be a Datapoint subscription then
    //

    {
	// make sure it's a Datapoint topic too
	if (strchr(topic, ':') == NULL) return -1;

        char sub_node_id[BIONET_NAME_COMPONENT_MAX_LEN];
        char sub_resource_id[BIONET_NAME_COMPONENT_MAX_LEN];

        char topic_node_id[BIONET_NAME_COMPONENT_MAX_LEN];
        char topic_resource_id[BIONET_NAME_COMPONENT_MAX_LEN];

        int r;

        r = bionet_split_nodeid_resourceid_r(subscription, sub_node_id, sub_resource_id);
        if (r != 0) return -1;

        r = bionet_split_nodeid_resourceid_r(topic, topic_node_id, topic_resource_id);
        if (r != 0) return -1;

        if (!bionet_name_component_matches(topic_node_id, sub_node_id)) return -1;
        if (!bionet_name_component_matches(topic_resource_id, sub_resource_id)) return -1;

        return 0;
    }
}

