
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>

#include "bionet-data-manager.h"


int libbdm_cal_topic_matches(const char *topic, const char *subscription) {

    //
    // There are four kinds of subscriptions: hab subscriptions, node subscriptions, stream
    // subscriptions, and datapoint subscriptions.
    //
    // Hab subscriptions are of the form "H <HabType>.<HabId>"
    //
    // Node subscriptions are of the form "N <HabType>.<HabId>.<NodeId>"
    //
    // Stream subscriptions are of the form "S <HabType>.<HabId>.<NodeID>:<StreamID>"
    //
    // Datapoint subscriptions are of the form "D <HabType>.<HabId>.<NodeID>:<ResourceID>"
    //


    //
    // first of all, the family of the topic of the subscription and the
    // family of the topic of the published info need to be the same
    //

    if (strncmp(subscription, topic, 2) != 0) return -1;


#if 0 
    //TODO: Support anything other than datapoint subscriptions
    //
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
#endif

    if (subscription[0] == 'D') {
        char sub_hab_type[BIONET_NAME_COMPONENT_MAX_LEN];
        char sub_hab_id[BIONET_NAME_COMPONENT_MAX_LEN];
        char sub_node_id[BIONET_NAME_COMPONENT_MAX_LEN];
        char sub_resource_id[BIONET_NAME_COMPONENT_MAX_LEN];

        char topic_hab_type[BIONET_NAME_COMPONENT_MAX_LEN];
        char topic_hab_id[BIONET_NAME_COMPONENT_MAX_LEN];
        char topic_node_id[BIONET_NAME_COMPONENT_MAX_LEN];
        char topic_resource_id[BIONET_NAME_COMPONENT_MAX_LEN];

        int sub_off, topic_off;

        sub_off = bionet_split_resource_name_r(&subscription[2], 
                sub_hab_type, sub_hab_id, sub_node_id, sub_resource_id);
        if (sub_off < 0) return -1;

        topic_off = bionet_split_resource_name_r(&topic[2], 
                topic_hab_type, topic_hab_id, topic_node_id, topic_resource_id);
        if (topic_off < 0) return -1;

        if (!bionet_name_component_matches(topic_hab_type, sub_hab_type)) return -1;
        if (!bionet_name_component_matches(topic_hab_id, sub_hab_id)) return -1;
        if (!bionet_name_component_matches(topic_node_id, sub_node_id)) return -1;
        if (!bionet_name_component_matches(topic_resource_id, sub_resource_id)) return -1;

        /*
        if(sub_off) {
            GHashTable * topic_params = NULL;
            GHashTable * sub_params = NULL;

            r = parse_topic_params(subscription + 2 + sub_off, &params);
            if (r != 0) return -1;

            r = parse_topic_params(topic + 2 + topic_off, &params);
            if (r != 0) return -1;

        }
        */




        return 0;
    }


    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "unknown publish topic '%s'", topic);
    return -1;
}

