
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include "bionet-data-manager.h"
#include "bdm-asn.h"
#include "cal-server.h"


static int bdm_resource_to_topic_str_r(const bionet_resource_t *resource, char topic_str[BDM_TOPIC_MAX_LEN], const char * bdm_id )
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
        snprintf(topic_str, BDM_TOPIC_MAX_LEN, "D %s/%s.%s?tsmin=%ld.%06ld&tsmax=%ld.%06ld", 
            bdm_id,
            bionet_hab_get_name(hab), 
            bionet_resource_get_local_name(resource),
            (long)tsmin->tv_sec, (long)tsmin->tv_usec,
            (long)tsmax->tv_sec, (long)tsmax->tv_usec);

        return 0;
    }

    return -1;
}

int bdm_report_new_hab(
        dbb_event_t * event,
        bionet_event_t * bionet_event)
{
    int r;
    bionet_asn_buffer_t buf;
    char topic[BDM_TOPIC_MAX_LEN];

    const char * bdm_id = event->recording_bdm->bdm_id;
    dbb_hab_t * dbb_hab = event->data.hab;

    snprintf(topic, sizeof(topic), "H %s/%s.%s", 
        bdm_id, dbb_hab->hab_type, dbb_hab->hab_id);


    r = bdm_new_hab_to_asnbuf(dbb_hab->hab_type, dbb_hab->hab_id,
            event->rowid, bionet_event_get_timestamp(bionet_event),
            bdm_id, &buf);
    if ( r != 0 ) {
        // An error has already been logged
        return -1;
    }

    // publish the message to any connected subscribers
    cal_server.publish(libbdm_cal_handle, topic, buf.buf, buf.size);
    // FIXME: cal_server.publish should take the buf
    free(buf.buf);

    bionet_event_free(bionet_event);

    return 0;
}

int bdm_report_lost_hab(
        dbb_event_t * event,
        bionet_event_t * bionet_event)
{
    int r;
    bionet_asn_buffer_t buf;
    char topic[BDM_TOPIC_MAX_LEN];

    const char * bdm_id = event->recording_bdm->bdm_id;
    dbb_hab_t * dbb_hab = event->data.hab;

    snprintf(topic, sizeof(topic), "H %s/%s.%s", 
        bdm_id, dbb_hab->hab_type, dbb_hab->hab_id);

    r = bdm_lost_hab_to_asnbuf(
            dbb_hab->hab_type, dbb_hab->hab_id,
            event->rowid, &event->timestamp, bdm_id,
            &buf);
    if ( r != 0 ) {
        // An error has already been logged
        return -1;
    }

    // publish the message to any connected subscribers
    cal_server.publish(libbdm_cal_handle, topic, buf.buf, buf.size);
    // FIXME: cal_server.publish should take the buf
    free(buf.buf);

    bionet_event_free(bionet_event);

    return 0;
}

int bdm_report_new_node(
        dbb_event_t * event,
        bionet_event_t * bionet_event)
{
    int r;
    bionet_asn_buffer_t buf;
    char topic[BDM_TOPIC_MAX_LEN];

    const char * bdm_id = event->recording_bdm->bdm_id;
    dbb_node_t * dbb_node = event->data.node;
    dbb_hab_t * dbb_hab = dbb_node->hab;

    bionet_hab_t * hab = bionet_hab_new(dbb_hab->hab_type, dbb_hab->hab_id);
    bionet_node_t * node = node_bdm_to_bionet(dbb_node, hab);
    bionet_hab_add_node(hab, node);
    bionet_node_add_event(node, bionet_event);

    snprintf(topic, sizeof(topic), "N %s/%s.%s", 
        bdm_id,
        bionet_hab_get_name(hab), 
        bionet_node_get_id(node));

    r = bdm_new_node_to_asnbuf(node, 
            bionet_node_get_event_by_index(node, 0),
            event->rowid, 
            &buf);
    if ( r != 0 ) {
        // An error has already been logged
        return -1;
    }


    // publish the message to any connected subscribers
    cal_server.publish(libbdm_cal_handle, topic, buf.buf, buf.size);
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

            r = bdm_resource_metadata_to_asnbuf(resource, event->rowid, &buf);
            if (r != 0) return -1;

            snprintf(topic, sizeof(topic), "D %s/%s.%s", 
                bdm_id,
                bionet_hab_get_name(hab), 
                bionet_resource_get_local_name(resource));

            // publish the message to any connected subscribers
            cal_server.publish(libbdm_cal_handle, topic, buf.buf, buf.size);

            // FIXME: cal_server.publish should take the buf
            free(buf.buf);

        }
    }
    bionet_hab_free(hab);
    

    return 0;
}

int bdm_report_lost_node(
        dbb_event_t * event,
        bionet_event_t * bionet_event)
{
    int r;
    bionet_asn_buffer_t buf;
    char topic[BDM_TOPIC_MAX_LEN];

    const char * bdm_id = event->recording_bdm->bdm_id;
    dbb_node_t * dbb_node = event->data.node;
    dbb_hab_t * dbb_hab = dbb_node->hab;

    bionet_hab_t * hab = bionet_hab_new(dbb_hab->hab_type, dbb_hab->hab_id);
    bionet_node_t * node = node_bdm_to_bionet(dbb_node, hab);
    bionet_hab_add_node(hab, node);
    bionet_node_add_event(node, bionet_event);

    snprintf(topic, sizeof(topic), "N %s/%s.%s.%s", 
        bdm_id, dbb_hab->hab_type, dbb_hab->hab_id,
        dbb_node->node_id);

    r = bdm_lost_node_to_asnbuf(node, 
            bionet_node_get_event_by_index(node, 0),
            event->rowid, 
            &buf);
    if ( r != 0 ) {
        // An error has already been logged
        return -1;
    }


    // publish the message to any connected subscribers
    cal_server.publish(libbdm_cal_handle, topic, buf.buf, buf.size);
    // FIXME: cal_server.publish should take the buf
    free(buf.buf);

    bionet_event_free(bionet_event);

    return 0;
}

int bdm_report_datapoint(
        dbb_event_t * event,
        bionet_event_t * bionet_event)
{
    bionet_asn_buffer_t buf;
    int r;
    char datapoint_topic[BDM_TOPIC_MAX_LEN];

    const char * bdm_id = event->recording_bdm->bdm_id;

    dbb_resource_t * dbb_resource = event->data.datapoint.resource;
    dbb_node_t * dbb_node = dbb_resource->node;
    dbb_hab_t * dbb_hab = dbb_node->hab;

    bionet_hab_t * hab = bionet_hab_new(dbb_hab->hab_type, dbb_hab->hab_id);
    bionet_node_t * node = node_bdm_to_bionet(dbb_node, hab);
    bionet_hab_add_node(hab, node);

    bionet_resource_t * resource = bionet_node_get_resource_by_id(node, dbb_resource->resource_id);

    bionet_datapoint_t * datapoint = datapoint_bdm_to_bionet(event->data.datapoint.dp, resource);
    bionet_datapoint_add_event(datapoint, bionet_event);
    bionet_resource_add_datapoint(resource, datapoint);

    if ( bdm_resource_to_topic_str_r(resource, datapoint_topic, bdm_id) == 0 ) {

        r = bdm_resource_datapoints_to_asnbuf(resource, event->rowid, &buf);
        if (r != 0) return -1;

        // publish the message to any connected subscribers
        cal_server.publish(libbdm_cal_handle, datapoint_topic, buf.buf, buf.size);

        // FIXME: cal_server.publish should take the buf
        free(buf.buf);
    }

    bionet_hab_free(hab);

    return 0;
}

