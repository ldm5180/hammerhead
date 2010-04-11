
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

// These structure defs are only needed here
struct dbb_bdm {
    char * bdm_id;
    GData * hab_list;
    int seq_index;
};

struct dbb_hab {
    char * hab_id;
    char * hab_type;
    dbb_bdm_t * recording_bdm;
    GData * node_list;
    int seq_index;
};

struct dbb_node {
    struct dbb_hab * hab;
    char * node_id;
    GData * resource_list;
    int seq_index;
};

struct dbb_resource {
    struct dbb_node * node;
    char * resource_id;
    bionet_resource_data_type_t data_type;
    bionet_resource_flavor_t flavor;
    uint8_t resource_key[BDM_RESOURCE_KEY_LENGTH];
    bionet_resource_t * bionet_resource;

    GSList *datapoint_list;
    int seq_index;
};


typedef struct {
    int entry_seq;
    int ret;
} dbb_foreach_t;

void dbb_datapoint_delete(void * data) {
    bdm_datapoint_t * dp = (bdm_datapoint_t*)data;

    if ( dp->type == DB_STRING) free(dp->value.str);
    free(dp);
}


bdm_datapoint_t * dbb_add_datapoint(bdm_db_batch *dbb, bionet_datapoint_t *bionet_datapoint, const char * bdm_id) {
    int r;
    bionet_value_t * bionet_value = NULL;
    bionet_resource_t * bionet_resource = NULL;
    bionet_node_t * bionet_node = NULL;
    bionet_hab_t * bionet_hab = NULL;

    bionet_value = bionet_datapoint_get_value(bionet_datapoint);
    bionet_resource = bionet_value_get_resource(bionet_value);
    bionet_node = bionet_resource_get_node(bionet_resource);
    bionet_hab = bionet_node_get_hab(bionet_node);

    dbb_bdm_t * bdm = dbb_add_bdm(dbb, bdm_id);
    if (bdm == NULL ) return NULL;

    dbb_hab_t * hab = dbb_add_hab(dbb, bionet_hab);
    if (hab == NULL ) return NULL;

    hab->recording_bdm = bdm;

    dbb_node_t * node = dbb_add_node(dbb, bionet_node);
    if (node == NULL ) return NULL;

    GQuark k = g_quark_from_string(bionet_resource_get_id(bionet_resource));

    dbb_resource_t * res = (dbb_resource_t*)g_datalist_id_get_data(&node->resource_list, k);

    assert(res != NULL); // dbb_add_node ensures this is true

    bdm_datapoint_t * dp = calloc(1, sizeof(bdm_datapoint_t));
    if(dp == NULL) return NULL;

    r = datapoint_bionet_to_bdm(bionet_datapoint, dp, bdm_id);
    if (r != 0) {
        dbb_datapoint_delete(dp);
        return NULL;
    }

    res->datapoint_list = g_slist_prepend(res->datapoint_list, dp);

    dp->seq_index = dbb->num_seq_needed++;

    return dp;
}

void dbb_node_delete(void *data) {
    dbb_node_t * node = (dbb_node_t*)data;

    free(node->node_id);
    g_datalist_clear(&node->resource_list);
    free(node);
}


void dbb_resource_delete(void *data) {
    dbb_resource_t * res = (dbb_resource_t*)data;

    free(res->resource_id);

    while(res->datapoint_list) {
        bdm_datapoint_t * dp = (bdm_datapoint_t*)res->datapoint_list->data;

        dbb_datapoint_delete(dp);
        res->datapoint_list = g_slist_delete_link(res->datapoint_list, res->datapoint_list);
    }

    free(res);
}


dbb_node_t * dbb_add_node(bdm_db_batch *dbb, bionet_node_t *bionet_node) {
    int i;
    bionet_hab_t * bionet_hab = NULL;
    GQuark k, rk;
    dbb_node_t * node;

    bionet_hab = bionet_node_get_hab(bionet_node);

    assert(bionet_hab != NULL);

    dbb_hab_t * hab = dbb_add_hab(dbb, bionet_hab);
    if (hab == NULL) return NULL;

    k = g_quark_from_string(bionet_node_get_id(bionet_node));

    node = (dbb_node_t*)g_datalist_id_get_data(&hab->node_list, k);

    if ( node == NULL ) {
        node = calloc(sizeof(dbb_node_t), 1);
        if ( NULL == node ) return NULL;

        node->hab = hab;
        node->node_id = strdup(bionet_node_get_id(bionet_node));

        g_datalist_id_set_data_full(&hab->node_list, k, node, dbb_node_delete);

        node->seq_index = dbb->num_seq_needed++;
    }


    // We re-cehck to make sure all resources are present in the off chance that
    // a hab removed then re-added a node with new resources
    for (i = 0; i < bionet_node_get_num_resources(bionet_node); i++) {
        bionet_resource_t *r = bionet_node_get_resource_by_index(bionet_node, i);
        //bionet_datapoint_t *d = bionet_resource_get_datapoint_by_index(r, 0);

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

            res->seq_index = dbb->num_seq_needed++;

            g_datalist_id_set_data_full(&node->resource_list, rk, res, dbb_resource_delete);
        }

    }

    return node;

}

void dbb_hab_delete(void * data) {
    dbb_hab_t * hab = (dbb_hab_t*)data;

    free(hab->hab_type);
    free(hab->hab_id);
    g_datalist_clear(&hab->node_list);
    free(hab);
}

dbb_hab_t * dbb_add_hab(bdm_db_batch *dbb, bionet_hab_t *bionet_hab) {
    const char * hab_name = bionet_hab_get_name(bionet_hab);

    GQuark k = g_quark_from_string(hab_name);

    dbb_hab_t * hab = (dbb_hab_t*)g_datalist_id_get_data(&dbb->hab_list, k);

    if ( hab == NULL ) {
        hab = calloc(sizeof(dbb_hab_t), 1);
        if ( NULL == hab ) return NULL;

        hab->hab_id = strdup(bionet_hab_get_id(bionet_hab));
        hab->hab_type = strdup(bionet_hab_get_type(bionet_hab));

        g_datalist_id_set_data_full(&dbb->hab_list, k, hab, dbb_hab_delete);

        hab->seq_index = dbb->num_seq_needed++;
    }

    const char * bdm_id = bionet_hab_get_recording_bdm(bionet_hab);
    if ( bdm_id ) {
        dbb_bdm_t * bdm = dbb_add_bdm(dbb, bdm_id);
        if (bdm == NULL ) return NULL;

        hab->recording_bdm = bdm;
    }


    return hab;
}

void dbb_bdm_delete(void* data) {
    dbb_bdm_t * bdm = (dbb_bdm_t*)data;

    free(bdm->bdm_id);
    free(bdm);
}

dbb_bdm_t * dbb_add_bdm(bdm_db_batch *dbb, const char * bdm_id) {
    

    GQuark k = g_quark_from_string(bdm_id);


    dbb_bdm_t * bdm = (dbb_bdm_t*)g_datalist_id_get_data(&dbb->bdm_list, k);

    if ( bdm == NULL ) {
        bdm = calloc(sizeof(dbb_bdm_t), 1);
        if ( NULL == bdm ) return NULL;

        bdm->bdm_id = strdup(bdm_id);
        g_datalist_id_set_data_full(&dbb->bdm_list, k, bdm, dbb_bdm_delete);

        // BDMs do not (yet) use sequence numbers...
        //dbb->num_seq_needed++;
    }

    return bdm;

}


static void _flush_foreach_bdm(GQuark key_id, void* data, void* user_data) {
    dbb_foreach_t * dat = (dbb_foreach_t*)user_data;
    dbb_bdm_t * bdm = (dbb_bdm_t*)data;

    if (db_add_bdm(main_db, bdm->bdm_id)) {
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

    dbb_node_t * node = resource->node;
    dbb_hab_t * hab = node->hab;


    r = db_insert_resource(main_db, 
            hab->hab_type, hab->hab_id, 
            node->node_id, 
            resource->resource_id, resource->flavor, resource->data_type,
            dat->entry_seq + resource->seq_index);
    if (r) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
              "%s(): Failed to add HAB %s to DB.",
              __FUNCTION__,
              resource->resource_id);
        dat->ret = -1;
        return;
    }

    // For performance, the list is prepended with new datapoints. 
    // Reverse it once here, so that we can efficiently traverse in the correct order
    GSList * dplist = g_slist_reverse(resource->datapoint_list);
    resource->datapoint_list = dplist;

    while(dplist) {
        bdm_datapoint_t * dp = (bdm_datapoint_t*)dplist->data;

        r = db_insert_datapoint(main_db, resource->resource_key, dp, 
                dat->entry_seq + dp->seq_index);
        if (r) {
            dat->ret = -1;
            return;
        }

        dplist = g_slist_next(dplist);
    }

    dat->ret = 0;

}

static void _publish_foreach_add_resource(GQuark key_id, void* data, void* user_data) {
    bionet_node_t * bionet_node = (bionet_node_t*)user_data;
    dbb_resource_t * resource = (dbb_resource_t*)data;

    bionet_resource_t * bionet_resource;

    bionet_resource = bionet_resource_new(bionet_node, 
            resource->data_type,
            resource->flavor,
            resource->resource_id);

    bionet_node_add_resource(bionet_node, bionet_resource);
    resource->bionet_resource = bionet_resource;
    bionet_resource_set_user_data(bionet_resource,(void*)resource);

}

static void _flush_foreach_node(GQuark key_id, void* data, void* user_data) {
    dbb_foreach_t * dat = (dbb_foreach_t*)user_data;
    dbb_node_t * node = (dbb_node_t*)data;
    dbb_hab_t * hab = node->hab;

    if (db_insert_node(main_db, node->node_id, hab->hab_type, hab->hab_id, dat->entry_seq + node->seq_index )) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
              "%s(): Failed to add HAB %s to DB.",
              __FUNCTION__,
              node->node_id);
        dat->ret = -1;
        return;
    }

    g_datalist_foreach(&node->resource_list, _flush_foreach_resource, user_data);

}

static void _publish_foreach_node(GQuark key_id, void* data, void* user_data) {
    dbb_foreach_t * dat = (dbb_foreach_t*)user_data;
    dbb_node_t * node = (dbb_node_t*)data;
    dbb_hab_t * hab = node->hab;

    bionet_hab_t * bionet_hab = bionet_hab_new(hab->hab_type, hab->hab_id);
    bionet_node_t * bionet_node = bionet_node_new(bionet_hab, node->node_id);
    if ( hab->recording_bdm ) {
        bionet_hab_set_recording_bdm(bionet_hab, hab->recording_bdm->bdm_id);
    }

    // Add the resources to the bionet_node
    g_datalist_foreach(&node->resource_list, _publish_foreach_add_resource, (void*)bionet_node);

    // publish this node and its resources
    bdm_report_new_node(bionet_node, dat->entry_seq + node->seq_index);


    // Publish the resource datapoints
    // walk list of bionet resources
    int ri;
    for (ri = 0; ri < bionet_node_get_num_resources(bionet_node); ri++) {
        bionet_resource_t * bionet_resource;
       
        bionet_resource = bionet_node_get_resource_by_index(bionet_node, ri);
        if (NULL == bionet_resource) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
                  "Failed to get resource %d from Node %s",
                  ri, bionet_node_get_name(bionet_node));
            continue;
        }

        dbb_resource_t * resource = bionet_resource_get_user_data(bionet_resource);

        GSList * dplist = resource->datapoint_list;
        for(; dplist != NULL; dplist = g_slist_next(dplist)) {
            bdm_datapoint_t * dp = (bdm_datapoint_t*)dplist->data;

            bionet_resource_remove_datapoint_by_index(bionet_resource, 0);
            bionet_resource_add_datapoint(
                    bionet_resource, 
                    datapoint_bdm_to_bionet(dp, bionet_resource));

            bdm_report_datapoints(bionet_resource, dat->entry_seq + dp->seq_index);
        }

        bionet_resource_set_user_data(bionet_resource, NULL);

    } //for each resource

    bionet_node_free(bionet_node);
    bionet_hab_free(bionet_hab);


}

static void _flush_foreach_hab(GQuark key_id, void* data, void* user_data) {
    dbb_foreach_t * dat = (dbb_foreach_t*)user_data;
    dbb_hab_t * hab = (dbb_hab_t*)data;

    if (db_insert_hab(main_db, hab->hab_type, hab->hab_id, dat->entry_seq + hab->seq_index)) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
              "%s(): Failed to add HAB %s.%s to DB.",
              __FUNCTION__,
              hab->hab_type, hab->hab_id);

        dat->ret = -1;
        return;
    }

    g_datalist_foreach(&hab->node_list, _flush_foreach_node, user_data);

}

static void _publish_foreach_hab(GQuark key_id, void* data, void* user_data) {
    dbb_foreach_t * dat = (dbb_foreach_t*)user_data;
    dbb_hab_t * hab = (dbb_hab_t*)data;


    bionet_hab_t * bionet_hab = bionet_hab_new(hab->hab_type, hab->hab_id);
    if ( hab->recording_bdm ) {
        bionet_hab_set_recording_bdm(bionet_hab, hab->recording_bdm->bdm_id);
    }

    bdm_report_new_hab(bionet_hab, dat->entry_seq + hab->seq_index);

    bionet_hab_free(bionet_hab);

    g_datalist_foreach(&hab->node_list, _publish_foreach_node, user_data);

}


int dbb_flush_to_db(bdm_db_batch * dbb) {
    dbb_foreach_t flush_dat = {0};
    flush_dat.ret = -1;
    int seq = -1;

    if ( dbb->bdm_list || dbb->hab_list ) {
        while(1) {
            seq = db_get_next_entry_seq_new_transaction(main_db, dbb->num_seq_needed);

            if(seq < 0) return -1;

            flush_dat.entry_seq = seq;

            g_datalist_foreach(&dbb->bdm_list, _flush_foreach_bdm, &flush_dat);
            if ( flush_dat.ret == 0 ) {
                g_datalist_foreach(&dbb->hab_list, _flush_foreach_hab, &flush_dat);
            }
            if ( flush_dat.ret == 0 ) {
                int r;
                r = db_commit(main_db);

                if ( r == 0 ) {
                    // Insert completed. Yeah!
                    break;
                }
            }

            // Try again. We had a conflict with another thread
            db_rollback(main_db);
        }

        dbb_foreach_t pub_dat = {0};
        pub_dat.entry_seq = seq;

        g_datalist_foreach(&dbb->hab_list, _publish_foreach_hab, &pub_dat);

        g_datalist_clear(&dbb->bdm_list);
        g_datalist_clear(&dbb->hab_list);

        dbb->num_seq_needed = 0;
    }

    return flush_dat.ret;

}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
