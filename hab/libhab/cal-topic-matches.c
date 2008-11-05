
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <string.h>

#include "libhab-internal.h"


int libhab_cal_topic_matches(const char *topic, const char *subscription) {

    //
    // There are two kinds of subscriptions: node subscriptions and
    // datapoint subscriptions.
    //
    // Node subscriptions are of the form <NodeId>.
    //
    // Datapoint subscriptions are of the form <NodeID:ResourceID>
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
    // this is a Datapoint subscription
    //


    // make sure it's a Datapoint topic too
    if (strchr(topic, ':') == NULL) return -1;


    {
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

