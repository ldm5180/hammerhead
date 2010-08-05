
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <bionet.h>
#include <glib.h>
#include <assert.h>

#include "bionet-data-manager.h"
#include "bdm-db.h"
#include "../../util/protected.h"

#define MAX_DB_COMMIT_ATTEMPT 10

/*
 * This file provides a DB Batch API that can be used by 
 * all components that want to insert into the database.
 *
 * It caches changes into a batch that can be inserted in 
 * one transaction for performance.
 */

// The return_resource is optional
bdm_datapoint_t * dbb_add_datapoint(
        bdm_db_batch_t *dbb,
        bionet_datapoint_t *bionet_datapoint,
        dbb_resource_t ** return_resource) ;

dbb_resource_t * dbb_add_resource(bdm_db_batch_t *dbb, bionet_resource_t *resource);
dbb_node_t * dbb_add_node(bdm_db_batch_t *dbb, bionet_node_t *node);
dbb_hab_t *  dbb_add_hab(bdm_db_batch_t *dbb, bionet_hab_t *hab);
dbb_bdm_t *  dbb_add_bdm(bdm_db_batch_t *dbb, const char * bdm_id);



/*
 * Structure used for "foreach" add callbacks
 */
typedef struct {
    int ret;
    
    int64_t first_seq;
    int64_t last_seq;
} dbb_foreach_t;


static void dbb_datapoint_delete(void * data) {
    bdm_datapoint_t * dp = (bdm_datapoint_t*)data;

    if ( dp->type == DB_STRING) free(dp->value.str);
    free(dp);
}


/*
 * Add the datapoint to the batch
 */
bdm_datapoint_t * dbb_add_datapoint(
        bdm_db_batch_t *dbb,
        bionet_datapoint_t *bionet_datapoint,
        dbb_resource_t ** return_resource) 
{
    int r;
    bionet_value_t * bionet_value = NULL;
    bionet_resource_t * bionet_resource = NULL;
    bionet_node_t * bionet_node = NULL;
    bionet_hab_t * bionet_hab = NULL;

    bionet_value = bionet_datapoint_get_value(bionet_datapoint);
    bionet_resource = bionet_value_get_resource(bionet_value);
    bionet_node = bionet_resource_get_node(bionet_resource);
    bionet_hab = bionet_node_get_hab(bionet_node);

    dbb_hab_t * hab = dbb_add_hab(dbb, bionet_hab);
    if (hab == NULL ) return NULL;

    dbb_node_t * node = dbb_add_node(dbb, bionet_node);
    if (node == NULL ) return NULL;

    GQuark k = g_quark_from_string(bionet_resource_get_id(bionet_resource));

    dbb_resource_t * res = (dbb_resource_t*)g_datalist_id_get_data(&node->resource_list, k);

    assert(res != NULL); // dbb_add_node ensures this is true

    bdm_datapoint_t * dp = calloc(1, sizeof(bdm_datapoint_t));
    if(dp == NULL) return NULL;

    r = datapoint_bionet_to_bdm(bionet_datapoint, dp);
    if (r != 0) {
        dbb_datapoint_delete(dp);
        return NULL;
    }

    res->datapoint_list = g_slist_prepend(res->datapoint_list, dp);

    if(return_resource){
        *return_resource = res;
    }

    return dp;
}

static void dbb_node_delete(void *data) {
    dbb_node_t * node = (dbb_node_t*)data;

    free(node->node_id);
    g_datalist_clear(&node->resource_list);
    free(node);
}


static void dbb_resource_delete(void *data) {
    dbb_resource_t * res = (dbb_resource_t*)data;

    free(res->resource_id);

    while(res->datapoint_list) {
        bdm_datapoint_t * dp = (bdm_datapoint_t*)res->datapoint_list->data;

        dbb_datapoint_delete(dp);
        res->datapoint_list = g_slist_delete_link(res->datapoint_list, res->datapoint_list);
    }

    free(res);
}

static dbb_node_t * dbb_hab_get_node(
        dbb_hab_t * hab, 
        const char * node_id,
        const uint8_t node_uid[BDM_UUID_LEN])
{
    GQuark k;
    dbb_node_t * node;

    char quark_str[(2*BDM_UUID_LEN) +  BIONET_NAME_COMPONENT_MAX_LEN + 2];

    snprintf(quark_str, sizeof(quark_str), UUID_FMTSTR " %s", 
            UUID_ARGS(node_uid), node_id);

    k = g_quark_from_string(quark_str);

    node = (dbb_node_t*)g_datalist_id_get_data(&hab->node_list, k);

    if ( node == NULL ) {
        node = calloc(sizeof(dbb_node_t), 1);
        if ( NULL == node ) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Out of Memory");
            return NULL;
        }
        g_datalist_init(&node->resource_list);

        memcpy(node->guid, node_uid, BDM_UUID_LEN);

        node->hab = hab;
        node->node_id = strdup(node_id);

        g_datalist_id_set_data_full(&hab->node_list, k, node, dbb_node_delete);
    }

    return node;
}

dbb_node_t * dbb_add_node(bdm_db_batch_t *dbb, bionet_node_t *bionet_node) {
    int i;
    bionet_hab_t * bionet_hab = NULL;
    dbb_node_t * node;
    GQuark rk;



    bionet_hab = bionet_node_get_hab(bionet_node);

    assert(bionet_hab != NULL);

    dbb_hab_t * hab = dbb_add_hab(dbb, bionet_hab);
    if (hab == NULL) return NULL;

    node = dbb_hab_get_node(hab, 
            bionet_node_get_id(bionet_node),
            bionet_node_get_uid(bionet_node));
    if (node == NULL) return NULL;

    for (i = 0; i < bionet_node_get_num_resources(bionet_node); i++) {
        bionet_resource_t *r = bionet_node_get_resource_by_index(bionet_node, i);

        rk = g_quark_from_string(bionet_resource_get_id(r));

        dbb_resource_t * res = (dbb_resource_t*)g_datalist_id_get_data(&node->resource_list, rk);
        if (res == NULL) {
            res = calloc(1, sizeof(dbb_resource_t));
            if ( res == NULL ) break;

            res->node = node;

            db_make_resource_key(
                bionet_hab_get_type(bionet_hab),
                bionet_hab_get_id(bionet_hab),
                bionet_node_get_id(bionet_node),
                bionet_resource_get_id(r),
                bionet_resource_get_data_type(r),
                bionet_resource_get_flavor(r),
                res->resource_key);

            res->resource_id = strdup(bionet_resource_get_id(r));
            res->data_type = bionet_resource_get_data_type(r);
            res->flavor = bionet_resource_get_flavor(r);

            g_datalist_id_set_data_full(&node->resource_list, rk, res, dbb_resource_delete);
        }

    }

    return node;

}

static void dbb_hab_delete(void * data) {
    dbb_hab_t * hab = (dbb_hab_t*)data;

    free(hab->hab_type);
    free(hab->hab_id);
    g_datalist_clear(&hab->node_list);
    free(hab);
}

dbb_hab_t * dbb_add_hab(bdm_db_batch_t *dbb, bionet_hab_t *bionet_hab) {
    const char * hab_name = bionet_hab_get_name(bionet_hab);

    GQuark k = g_quark_from_string(hab_name);

    dbb_hab_t * hab = (dbb_hab_t*)g_datalist_id_get_data(&dbb->hab_list, k);

    if ( hab == NULL ) {
        hab = calloc(sizeof(dbb_hab_t), 1);
        if ( NULL == hab ) return NULL;

        hab->hab_id = strdup(bionet_hab_get_id(bionet_hab));
        hab->hab_type = strdup(bionet_hab_get_type(bionet_hab));

        g_datalist_id_set_data_full(&dbb->hab_list, k, hab, dbb_hab_delete);
    }


    return hab;
}

static void dbb_bdm_delete(void* data) {
    dbb_bdm_t * bdm = (dbb_bdm_t*)data;

    free(bdm->bdm_id);
    free(bdm);
}

dbb_bdm_t * dbb_add_bdm(bdm_db_batch_t *dbb, const char * bdm_id) {
    

    GQuark k = g_quark_from_string(bdm_id);


    dbb_bdm_t * bdm = (dbb_bdm_t*)g_datalist_id_get_data(&dbb->bdm_list, k);

    if ( bdm == NULL ) {
        bdm = calloc(sizeof(dbb_bdm_t), 1);
        if ( NULL == bdm ) return NULL;

        bdm->bdm_id = strdup(bdm_id);
        g_datalist_id_set_data_full(&dbb->bdm_list, k, bdm, dbb_bdm_delete);

    }

    return bdm;

}



/*
 * Add the datapoint to the batch
 */
dbb_event_t * dbb_add_event(
        bdm_db_batch_t *dbb,
        dbb_event_type_t type,
        dbb_bionet_event_data_t bionet_ptr,
        const char * bdm_id,
        const struct timeval *timestamp)
{
    //Create new event

    dbb_event_t * event = calloc(1, sizeof(dbb_event_t));
    if(event == NULL) return NULL;

    dbb_bdm_t * bdm = dbb_add_bdm(dbb, bdm_id);
    if (bdm == NULL ) {
        free(event);
        return NULL;
    }

    event->type = type;
    event->recording_bdm = bdm;

    event->timestamp.tv_sec = timestamp->tv_sec;
    event->timestamp.tv_usec = timestamp->tv_usec;


    // Add metadata
    switch(type) {
        case DBB_LOST_HAB_EVENT:
        case DBB_NEW_HAB_EVENT:
            {
                dbb_hab_t * hab = dbb_add_hab(dbb, bionet_ptr.hab);
                if(NULL == hab) goto fail;

                event->data.hab = hab;

                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
                        "%s(): new/lost hab: %s.%s", __FUNCTION__,
                        hab->hab_type, hab->hab_id);
            }
            break;

        case DBB_LOST_NODE_EVENT:
        case DBB_NEW_NODE_EVENT:
            {
                dbb_node_t * node = dbb_add_node(dbb, bionet_ptr.node);
                if ( NULL == node ) goto fail;

                event->data.node = node;

                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
                        "%s(): new/lost node: %s.%s.%s", __FUNCTION__,
                            node->hab->hab_type,
                            node->hab->hab_id,
                            node->node_id);
            }
            break;

        case DBB_DATAPOINT_EVENT:
            {
                dbb_resource_t * resource = NULL;
                bdm_datapoint_t * dp = dbb_add_datapoint(dbb, bionet_ptr.datapoint, &resource);
                if ( NULL == dp ) goto fail;

                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
                        "%s(): datapoint: %s", __FUNCTION__,
                        resource->resource_id);

                event->data.datapoint.resource = resource;
                event->data.datapoint.dp = dp;
            }
            break;

        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
                    "Unknown db-batch event type");
            goto fail;
    }

    dbb->event_list = g_list_prepend(dbb->event_list, event);
    
    return event;

fail:
    free(event);
    return NULL;
}





/*****  Flush and Publish *****/


static void _flush_foreach_bdm(GQuark key_id, void* data, void* user_data) {
    dbb_foreach_t * dat = (dbb_foreach_t*)user_data;
    dbb_bdm_t * bdm = (dbb_bdm_t*)data;

    if(bdm->rowid) return;

    if (db_insert_bdm(main_db, bdm->bdm_id, &bdm->rowid) < 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
              "%s(): Failed to add BDM %s to DB.",
              __FUNCTION__,
              bdm->bdm_id);

        dat->ret = -1;
    }

    dat->ret = 0;

}

static void _flush_foreach_resource(GQuark key_id, void* data, void* user_data) {
    dbb_foreach_t * dat = (dbb_foreach_t*)user_data;
    int r;
    dbb_resource_t * resource = (dbb_resource_t*)data;

    if(resource->rowid) return; //Already inserted

    dbb_node_t * node = resource->node;
    dbb_hab_t * hab = node->hab;


    r = db_insert_resource(main_db, 
            hab->hab_type, hab->hab_id, 
            node->node_id, node->guid,
            resource->resource_id, resource->flavor, resource->data_type,
            &resource->rowid);
    if (r < 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
              "%s(): Failed to add resource %s.%s.%s:%s to DB.",
              __FUNCTION__, hab->hab_type, hab->hab_id, 
              node->node_id, resource->resource_id);
        dat->ret = -1;
        return;
    }

    // For performance, the list is prepended with new datapoints. 
    // Reverse it once here, so that we can efficiently traverse in the correct order
    GSList * dplist = resource->datapoint_list;
    if(!resource->datapoint_list_reversed) {
        dplist = g_slist_reverse(resource->datapoint_list);
        resource->datapoint_list = dplist;
        resource->datapoint_list_reversed = 1;
    }

    for(; dplist != NULL; dplist = g_slist_next(dplist))
    {
        bdm_datapoint_t * dp = (bdm_datapoint_t*)dplist->data;

        if(dp->rowid) continue;

        r = db_insert_datapoint(main_db, resource->resource_key, dp, &dp->rowid);
        if (r < 0) {
            dat->ret = -1;
            return;
        }
    }

    dat->ret = 0;

}

static void _flush_foreach_node(GQuark key_id, void* data, void* user_data) {
    dbb_foreach_t * dat = (dbb_foreach_t*)user_data;
    dbb_node_t * node = (dbb_node_t*)data;
    dbb_hab_t * hab = node->hab;

    if( node->rowid ) return; // Already inserted

    if (db_insert_node(main_db, node->node_id, hab->hab_type, hab->hab_id, node->guid, &node->rowid) < 0)
    {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
              "%s(): Failed to add HAB %s to DB.",
              __FUNCTION__,
              node->node_id);
        dat->ret = -1;
        return;
    }

    g_datalist_foreach(&node->resource_list, _flush_foreach_resource, user_data);

}

static void _flush_foreach_hab(GQuark key_id, void* data, void* user_data) {
    dbb_foreach_t * dat = (dbb_foreach_t*)user_data;
    dbb_hab_t * hab = (dbb_hab_t*)data;

    if(hab->rowid) {
        // This was inserted before...
        return;
    }
                    
    if (db_insert_hab(main_db, hab->hab_type, hab->hab_id, &hab->rowid) < 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
              "%s(): Failed to add HAB %s.%s to DB.",
              __FUNCTION__,
              hab->hab_type, hab->hab_id);

        dat->ret = -1;
        return;
    }

    g_datalist_foreach(&hab->node_list, _flush_foreach_node, user_data);

}

static void _flush_foreach_event(void *data, void *user_data) {
    dbb_foreach_t * dat = (dbb_foreach_t*)user_data;
    dbb_event_t * event = (dbb_event_t*)data;

    sqlite3_int64 rowid;
    sqlite3_int64 data_rowid;

    dbb_hab_t * hab = NULL;
    dbb_node_t * node = NULL;
    bdm_datapoint_t * dp = NULL;

    switch(event->type) {
        case DBB_NEW_HAB_EVENT:
        case DBB_LOST_HAB_EVENT:
            hab = event->data.hab;
            data_rowid = hab->rowid;
            break;

        case DBB_NEW_NODE_EVENT:
        case DBB_LOST_NODE_EVENT:
            node = event->data.node;
            hab = node->hab;
            data_rowid = node->rowid;
            break;

        case DBB_DATAPOINT_EVENT:
            dp = event->data.datapoint.dp;
            data_rowid = dp->rowid;
            break;
    }

    if (db_insert_event(main_db, 
                &event->timestamp, event->recording_bdm->rowid, event->type, data_rowid, &rowid) < 0)
    {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
              "%s(): Failed to add event to DB.",
              __FUNCTION__);

        dat->ret = -1;
        return;
    }
    event->rowid = rowid;

    if(dat->first_seq < 0) {
        dat->first_seq = rowid;
    }
    dat->last_seq = rowid;

} 

static void _publish_foreach_event(void *data, void *user_data) {
    dbb_event_t * event = (dbb_event_t*)data;

    const char * bdm_id = event->recording_bdm->bdm_id;
    bionet_event_type_t event_type;



    switch(event->type) {
        case DBB_LOST_HAB_EVENT:
        case DBB_LOST_NODE_EVENT:
            event_type = BIONET_EVENT_LOST;
            break;

        case DBB_NEW_HAB_EVENT: 
        case DBB_NEW_NODE_EVENT: 
        case DBB_DATAPOINT_EVENT:
            event_type = BIONET_EVENT_PUBLISHED;
            break;
    }
    bionet_event_t * bionet_event = bionet_event_new(
            &event->timestamp,
            bdm_id,
            event_type);

    // publish the thing
    switch(event->type) {
        case DBB_LOST_HAB_EVENT:
            bdm_report_lost_hab(event, bionet_event);
            break;

        case DBB_NEW_HAB_EVENT: 
            bdm_report_new_hab(event, bionet_event);
            break;

        case DBB_LOST_NODE_EVENT:
            bdm_report_lost_node(event, bionet_event);
            break;

        case DBB_NEW_NODE_EVENT: 
            bdm_report_new_node(event, bionet_event);
            break;

        case DBB_DATAPOINT_EVENT:
            bdm_report_datapoint(event, bionet_event);
            break;
    }

} 

static void _reset_foreach_resource(GQuark key_id, void* data, void* user_data) {
    dbb_resource_t * resource = (dbb_resource_t*)data;

    resource->rowid = 0;

    GSList * dplist = resource->datapoint_list;
    for(; dplist != NULL; dplist = g_slist_next(dplist))
    {
        bdm_datapoint_t * dp = (bdm_datapoint_t*)dplist->data;
        dp->rowid = 0;
    }
}
static void _reset_foreach_node(GQuark key_id, void* data, void* user_data) {
    dbb_node_t * node = (dbb_node_t*)data;

    node->rowid = 0;
    g_datalist_foreach(&node->resource_list, _reset_foreach_resource, user_data);

}
static void _reset_foreach_hab(GQuark key_id, void* data, void* user_data) {
    dbb_hab_t * hab = (dbb_hab_t*)data;
    hab->rowid = 0;
    g_datalist_foreach(&hab->node_list, _reset_foreach_node, user_data);
}

static void _reset_foreach_bdm(GQuark key_id, void* data, void* user_data) {
    dbb_bdm_t * bdm = (dbb_bdm_t*)data;
    bdm->rowid = 0;
}
static void _reset_foreach_event(void *data, void *user_data) {
    dbb_event_t * event = (dbb_event_t*)data;
    event->rowid = 0;
}

void dbb_reset_rowcache(bdm_db_batch_t * dbb) {
        g_datalist_foreach(&dbb->bdm_list, _reset_foreach_bdm, NULL);
        g_datalist_foreach(&dbb->hab_list, _reset_foreach_hab, NULL);
        g_list_foreach(dbb->event_list, _reset_foreach_event, NULL);
}

void dbb_free(bdm_db_batch_t * dbb) {
    GList * item;

    g_datalist_clear(&dbb->bdm_list);
    g_datalist_clear(&dbb->hab_list);

    for(item=dbb->event_list; item != NULL; item = g_list_next(item) )
    {
        dbb_event_t * event = item->data;
        free(event);
        item->data = NULL;
    }
    g_list_free(dbb->event_list);
    dbb->event_list = NULL;

    free(dbb);
}

int dbb_flush_to_db(bdm_db_batch_t * dbb) {
    GList * item;
    int i;

    if(dbb->event_list == NULL) {
        return 0;
    }

    // The newest event is first. Reverse now to efficiently walk from 
    // oldest to newest
    dbb->event_list = g_list_reverse(dbb->event_list);

    dbb_foreach_t flush_dat = {0};
    for(i=0; i<MAX_DB_COMMIT_ATTEMPT; i++) {
        flush_dat.ret = -1;
        flush_dat.first_seq = -1;
        flush_dat.last_seq = -1;

        db_begin_transaction(main_db);

        g_datalist_foreach(&dbb->bdm_list, _flush_foreach_bdm, &flush_dat);
        if ( flush_dat.ret ) goto retry_flush;

        g_datalist_foreach(&dbb->hab_list, _flush_foreach_hab, &flush_dat);
        if ( flush_dat.ret ) goto retry_flush;

        g_list_foreach(dbb->event_list, _flush_foreach_event, &flush_dat);
        if ( flush_dat.ret ) goto retry_flush;

        if( 0 == db_commit(main_db) ) {
            // Insert completed. Yeah!
            break;
        }

retry_flush:
        // Try again. We had a conflict with another thread
        dbb_reset_rowcache(dbb);
        db_rollback(main_db);
    }

    if(i == MAX_DB_COMMIT_ATTEMPT) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "Failed to commit batch %d times", i);
        return -1;
    }

    dbb->first_seq = flush_dat.first_seq;
    dbb->last_seq = flush_dat.last_seq;

    // Data safely in DB.
    // Now publish it
    dbb_foreach_t pub_dat = {0};

    g_list_foreach(dbb->event_list, _publish_foreach_event, &pub_dat);


    g_datalist_clear(&dbb->bdm_list);
    g_datalist_clear(&dbb->hab_list);
    for(item=dbb->event_list; item != NULL; item = g_list_next(item) )
    {
        dbb_event_t * event = item->data;
        free(event);
        item->data = NULL;
    }
    g_list_free(dbb->event_list);
    dbb->event_list = NULL;

    return 0;
}

static void _foreach_add_resource(GQuark key_id, void* data, void* user_data) {
    bionet_node_t * bionet_node = (bionet_node_t*)user_data;
    dbb_resource_t * resource = (dbb_resource_t*)data;

    bionet_resource_t * bionet_resource;

    bionet_resource = bionet_resource_new(bionet_node, 
            resource->data_type,
            resource->flavor,
            resource->resource_id);

    bionet_node_add_resource(bionet_node, bionet_resource);
    resource->bionet_resource = bionet_resource;

}

bionet_node_t * node_bdm_to_bionet(dbb_node_t * dbb_node, bionet_hab_t * hab) {
    bionet_node_t * node = bionet_node_new(hab, dbb_node->node_id);
    if(node == NULL) return NULL;

    g_datalist_foreach(&dbb_node->resource_list, _foreach_add_resource, node);

    bionet_node_set_uid(node, dbb_node->guid);

    return node;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
