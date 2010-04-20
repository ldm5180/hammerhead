
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
};

struct dbb_hab {
    char * hab_id;
    char * hab_type;
    GData * node_list;
};

struct dbb_node {
    struct dbb_hab * hab;
    char * node_id;
    GData * resource_list;
};

struct dbb_resource {
    struct dbb_node * node;
    char * resource_id;
    bionet_resource_data_type_t data_type;
    bionet_resource_flavor_t flavor;
    uint8_t resource_key[BDM_RESOURCE_KEY_LENGTH];

    GSList *datapoint_list;
};


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

            g_datalist_id_set_data_full(&node->resource_list, rk, res, dbb_resource_delete);
        }

        // add the resource's data point, if any
        /*
        if (d != NULL) {
            r = dbb_add_datapoint(dbb, d, NULL);
            if (r != 0) return -1;
        }
        */
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
    }

    return bdm;

}


static void _flush_foreach_bdm(GQuark key_id, void* data, void* user_data) {
    dbb_bdm_t * bdm = (dbb_bdm_t*)data;

    if (db_add_bdm(main_db, bdm->bdm_id)) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
              "%s(): Failed to add BDM %s to DB.",
              __FUNCTION__,
              bdm->bdm_id);
    }

}

static void _flush_foreach_resource(GQuark key_id, void* data, void* user_data) {
    int r;
    dbb_resource_t * resource = (dbb_resource_t*)data;

    dbb_node_t * node = resource->node;
    dbb_hab_t * hab = node->hab;


    r = db_insert_resource(main_db, 
            hab->hab_type, hab->hab_id, 
            node->node_id, 
            resource->resource_id, resource->flavor, resource->data_type);
    if (r) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
              "%s(): Failed to add HAB %s to DB.",
              __FUNCTION__,
              resource->resource_id);
        *(int*)user_data = -1;
        return;
    }

    GSList * dplist = g_slist_reverse(resource->datapoint_list);
    resource->datapoint_list = dplist;
    while(dplist) {
        bdm_datapoint_t * dp = (bdm_datapoint_t*)dplist->data;

        r = db_insert_datapoint(main_db, resource->resource_key, dp);
        if (r) {
            *(int*)user_data = -1;
            return;
        }

        dplist = g_slist_next(dplist);
    }

}

static void _flush_foreach_node(GQuark key_id, void* data, void* user_data) {
    dbb_node_t * node = (dbb_node_t*)data;
    dbb_hab_t * hab = node->hab;

    if (db_insert_node(main_db, node->node_id, hab->hab_type, hab->hab_id )) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
              "%s(): Failed to add HAB %s to DB.",
              __FUNCTION__,
              node->node_id);
        *(int*)user_data = -1;
        return;
    }

    g_datalist_foreach(&node->resource_list, _flush_foreach_resource, user_data);

}

static void _flush_foreach_hab(GQuark key_id, void* data, void* user_data) {
    dbb_hab_t * hab = (dbb_hab_t*)data;

    if (db_insert_hab(main_db, hab->hab_type, hab->hab_id)) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
              "%s(): Failed to add HAB %s.%s to DB.",
              __FUNCTION__,
              hab->hab_type, hab->hab_id);

        *(int*)user_data = -1;
        return;
    }

    g_datalist_foreach(&hab->node_list, _flush_foreach_node, user_data);

}


int dbb_flush_to_db(bdm_db_batch * dbb) {
    int r = -1;

    if ( dbb->bdm_list || dbb->hab_list ) {
        db_get_next_entry_seq(main_db);

        // Start transaction
        r = db_begin_transaction(main_db);


        g_datalist_foreach(&dbb->bdm_list, _flush_foreach_bdm, &r);

        g_datalist_foreach(&dbb->hab_list, _flush_foreach_hab, &r);

        if ( r < 0 ) {
            db_rollback(main_db);
        } else {
            r = db_commit(main_db);
        }


        g_datalist_clear(&dbb->bdm_list);
        g_datalist_clear(&dbb->hab_list);
    }

    return r;
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
