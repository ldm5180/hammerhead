
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include "bionet-data-manager.h"
#include "bdm-asn.h"
#include "cal-server.h"


static int bdm_resource_to_topic_str_r(const bionet_resource_t *resource, char topic_str[BDM_TOPIC_MAX_LEN] )
{
    int num_dp;
    bionet_hab_t * hab = bionet_resource_get_hab(resource);

    num_dp = bionet_resource_get_num_datapoints(resource);
    if ( num_dp > 0 ) {
        struct timeval *tsmin, *tsmax;
        int j;
        tsmin = tsmax = bionet_datapoint_get_timestamp(bionet_resource_get_datapoint_by_index(resource, 0));
        for ( j=0; j<num_dp; j++) {
            struct timeval *ts =  bionet_datapoint_get_timestamp(bionet_resource_get_datapoint_by_index(resource, j));
            if (bionet_timeval_compare(ts, tsmin) < 0 ) tsmin = ts;
            if (bionet_timeval_compare(ts, tsmax) > 0 ) tsmax = ts;
        }
        snprintf(topic_str, BDM_TOPIC_MAX_LEN, "D %s.%s?tsmin=%ld.%06ld&tsmax=%ld.%06ld", 
            bionet_hab_get_name(hab), 
            bionet_resource_get_local_name(resource),
            (long)tsmin->tv_sec, (long)tsmin->tv_usec,
            (long)tsmax->tv_sec, (long)tsmax->tv_usec);

        return 0;
    }

    return -1;
}

int bdm_report_new_hab(
        bionet_bdm_t * bdm,
        bionet_hab_t * hab,
        int entry_seq) 
{
    int r;
    bionet_asn_buffer_t buf;
    char topic[BDM_TOPIC_MAX_LEN];

    snprintf(topic, sizeof(topic), "H %s", 
        bionet_hab_get_name(hab));

    r = bdm_new_hab_to_asnbuf(bdm, hab, entry_seq, &buf);
    if ( r != 0 ) {
        // An error has already been logged
        return -1;
    }

    // publish the message to any connected subscribers
    cal_server.publish(topic, buf.buf, buf.size);
    // FIXME: cal_server.publish should take the buf
    free(buf.buf);

    return 0;
}

int bdm_report_new_node(
        bionet_bdm_t * bdm,
        bionet_node_t * node,
        int entry_seq) 
{
    int r;
    bionet_asn_buffer_t buf;
    char topic[BDM_TOPIC_MAX_LEN];

    bionet_hab_t * hab = bionet_node_get_hab(node);

    snprintf(topic, sizeof(topic), "N %s.%s", 
        bionet_hab_get_name(hab), 
        bionet_node_get_id(node));

    r = bdm_new_node_to_asnbuf(bdm, node, entry_seq, &buf);
    if ( r != 0 ) {
        // An error has already been logged
        return -1;
    }

    // publish the message to any connected subscribers
    cal_server.publish(topic, buf.buf, buf.size);
    // FIXME: cal_server.publish should take the buf
    free(buf.buf);


    //
    // send this Node's Resources' metadata to datapoint subscribers
    // 
    {
        int ri;

        for (ri = 0; ri < bionet_node_get_num_resources(node); ri ++) {
            bionet_resource_t *resource = bionet_node_get_resource_by_index(node, ri);
            int r;

            r = bdm_resource_metadata_to_asnbuf(bdm, resource, entry_seq, &buf);
            if (r != 0) return -1;

            snprintf(topic, sizeof(topic), "D %s.%s", 
                bionet_hab_get_name(hab), 
                bionet_resource_get_local_name(resource));

            // publish the message to any connected subscribers
            cal_server.publish(topic, buf.buf, buf.size);

            // FIXME: cal_server.publish should take the buf
            free(buf.buf);


            if ( bdm_resource_to_topic_str_r(resource, topic) ) {
                // send all datapoints
                r = bdm_resource_datapoints_to_asnbuf(bdm, resource, entry_seq, &buf);
                if (r != 0) continue;

                // publish the message to any connected subscribers
                cal_server.publish(topic, buf.buf, buf.size);

                // FIXME: cal_server.publish should take the buf
                free(buf.buf);
            }
        }
    }
    

    return 0;
}

int bdm_report_datapoint(
        bionet_bdm_t * bdm,
        bionet_resource_t * resource,
        bionet_datapoint_t * datapoint,
        int entry_seq) 
{
    bionet_asn_buffer_t buf;
    int r;
    char datapoint_topic[BDM_TOPIC_MAX_LEN];

    if ( bdm_resource_to_topic_str_r(resource, datapoint_topic) == 0 ) {

        r = bdm_resource_datapoints_to_asnbuf(bdm, resource, entry_seq, &buf);
        if (r != 0) return -1;


        // publish the message to any connected subscribers
        cal_server.publish(datapoint_topic, buf.buf, buf.size);

        // FIXME: cal_server.publish should take the buf
        free(buf.buf);
    }

    return 0;
}

