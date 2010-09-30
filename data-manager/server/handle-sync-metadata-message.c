
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "bionet-data-manager.h"
#include "bdm-db.h"
#include "util/protected.h"

#include "bdm-stats.h"

#include "asn_SEQUENCE_OF.h"
typedef A_SEQUENCE_OF(struct BDM_Event) sequence_of_BDM_Event_t;

//struct BDM_HardwareAbstractor__events *events,

static int _dbb_add_asn_event(
        bdm_db_batch_t *dbb,
        const char * bdm_id,
        dbb_bionet_event_data_t bionet_ptr,
        dbb_event_type_t new_event_type,
        dbb_event_type_t lost_event_type,
        sequence_of_BDM_Event_t *event_sequence)
{
    int ei;
    int r = 0;
    for( ei = 0; ei < event_sequence->count; ei++) {
        BDM_Event_t * asn_event = event_sequence->array[ei];
        dbb_event_t * event;
        struct timeval ts;
        bionet_GeneralizedTime_to_timeval(&asn_event->timestamp, &ts);

        event = dbb_add_event(dbb, 
                asn_event->type == BDM_Event_Type_new?new_event_type:lost_event_type,
                bionet_ptr, 
                bdm_id, 
                &ts);

        if(event == NULL) {
            r = -1;
            break;
        }
        num_sync_recv_events++;
    }

    return r;
}

void handle_sync_metadata_message(BDM_Sync_Metadata_Message_t *message) {
    int bi;
    int r;

    bionet_hab_t * hab = NULL;

    dbb_bionet_event_data_t bionet_ptr;
    bdm_db_batch_t * tmp_dbb = calloc(1, sizeof(bdm_db_batch_t));
    if(tmp_dbb == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): Out of Memory!", __FUNCTION__);
        return;
    }

    int num_events_before = num_sync_recv_events;

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Sync Metadata Message {");

    for (bi = 0; bi < message->list.count; bi ++) {
        DataManager_t *asn_bdm;
        const char * bdm_id;
        int hi;

        asn_bdm = message->list.array[bi];
        bdm_id = (const char*)asn_bdm->id.buf;

        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "  BDM: %s", bdm_id);

        for (hi = 0; hi < asn_bdm->hablist.list.count; hi ++) {
            BDM_HardwareAbstractor_t *asn_hab;
            const char * hab_type;
            const char * hab_id;
            int ni;

            asn_hab = asn_bdm->hablist.list.array[hi];

            hab_id = (const char*)asn_hab->id.buf;
            hab_type = (const char*)asn_hab->type.buf;

            hab = bionet_hab_new(hab_type, hab_id);
            if ( NULL == hab) goto cleanup;

            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "    HAB: %s.%s", hab_type, hab_id);

            bionet_ptr.hab = hab;
            r = _dbb_add_asn_event(tmp_dbb, bdm_id, bionet_ptr,
                    DBB_NEW_HAB_EVENT, DBB_LOST_HAB_EVENT,
                    (sequence_of_BDM_Event_t*)&asn_hab->events.list);
            if( r != 0 ) goto cleanup;

            for (ni = 0; ni < asn_hab->nodes.list.count; ni ++) {
                int ri;
                const char * node_id;
                BDM_Node_t *asn_node;
                bionet_node_t * node;

                asn_node = asn_hab->nodes.list.array[ni];

                node_id = (const char*)asn_node->id.buf;

                if(BDM_UUID_LEN != asn_node->uid.size) {
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Unexpected length for node uid");
                    goto cleanup;
                }

                node = bionet_node_new(hab, node_id);
                if ( NULL == node ) goto cleanup;

                bionet_node_set_uid(node, asn_node->uid.buf);

                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "        Node: %s [" UUID_FMTSTR "]", 
                        node_id, UUID_ARGS(asn_node->uid.buf));

                for (ri = 0; ri < asn_node->resources.list.count; ri ++) {
                    BDM_Resource_t *asn_resource;
                    bionet_resource_data_type_t datatype;
                    bionet_resource_flavor_t flavor;
                    bionet_resource_t * resource;
                    const char * resource_id;
                    int di;

                    asn_resource = asn_node->resources.list.array[ri];

                    datatype = bionet_asn_to_datatype(asn_resource->datatype);
                    if (datatype == -1) {
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                              "Unknown datatype.");
                        bionet_node_free(node);
                        goto cleanup;
                    }

                    flavor = bionet_asn_to_flavor(asn_resource->flavor);
                    if (flavor == -1) {
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                              "Unknown flavor.");
                        bionet_node_free(node);
                        goto cleanup;
                    }

                    resource_id = (const char *)asn_resource->id.buf;

                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "            Resource: %s", resource_id);
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "                datatype: %s", 
                          bionet_resource_data_type_to_string(datatype));
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "                flavor: %s", 
                          bionet_resource_flavor_to_string(flavor));

                    resource = bionet_resource_new(node, datatype, flavor, resource_id);
                    if ( NULL == resource ) {
                        bionet_node_free(node);
                        goto cleanup;
                    }

                    if(bionet_node_add_resource(node, resource)) {
                        bionet_node_free(node);
                        bionet_resource_free(resource);
                        goto cleanup;
                    }

                    for (di = 0; di < asn_resource->datapoints.list.count; di++) {
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                              "Datapoints should never be in a sync metadata message. %d found.", 
                              asn_resource->datapoints.list.count);
                    }
                }

                bionet_ptr.node = node;
                r = _dbb_add_asn_event(tmp_dbb, bdm_id, bionet_ptr,
                        DBB_NEW_NODE_EVENT, DBB_LOST_NODE_EVENT,
                        (sequence_of_BDM_Event_t*)&asn_node->events.list);
                bionet_node_free(node);

                if( r != 0 ) goto cleanup;
            }

            bionet_hab_free(hab);
            hab = NULL;
        }
    }

    r = dbb_flush_to_db(tmp_dbb);
    if (r) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "} Sync message discarded due to error");
        goto cleanup;
    }

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "} Sync Metadata Message [%lld,%lld]",
        tmp_dbb->first_seq, tmp_dbb->last_seq);

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
          "Sync-Received %d events", num_sync_recv_events - num_events_before);


    if(tmp_dbb->first_seq >= 0 && tmp_dbb->last_seq >= 0 ) {
        db_publish_sync_affected_datapoints(main_db, tmp_dbb->first_seq, tmp_dbb->last_seq);
    }

cleanup:
    if(hab) {
        bionet_hab_free(hab);
    }

    dbb_free(tmp_dbb);

}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
