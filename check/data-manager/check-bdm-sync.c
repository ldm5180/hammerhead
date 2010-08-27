
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <check.h>

#include <glib.h>

#include "check-common.h"
#include "bionet-util.h"
#include "check-bdm.h"
#include "bionet-util.h"
#include "protected.h"
#include "data-manager/server/bionet-data-manager.h"
#include "data-manager/server/bdm-list-iterator.h"

#define SIZEOF(x) (sizeof(x)/sizeof(x[0]))

static const char * bdm_names[] = {
    "bdm1",
    "bdm2",
    "bdm3",
    "bdm4",
};
static const int num_bdm_names = SIZEOF(bdm_names);

static const char * hab_names[] = {
    "hab1",
    "hab2",
    "hab3",
    "hab4",
};
static const int num_hab_names = SIZEOF(hab_names);

static const char * node_names[] = {
    "node1",
    "node2",
    "node3",
    "node4",
};
static const int num_node_names = SIZEOF(node_names);

static const char * resource_names[] = {
    "resource1",
    "resource2",
    "resource3",
    "resource4",
};
static const int num_resource_names = SIZEOF(resource_names);

static const int MTU = 100000;

static int check_count_bytes(const void *buffer, size_t size, void * voidp) {
    ssize_t *pBytes = (ssize_t*)voidp;

    (*pBytes) += size;

    int i;
    char byte = 0;
    for(i=0; i<size; i++) {
        byte ^= ((char*)buffer)[i];
    }

    return size;
}

static GPtrArray * make_bdm_list(
        int num_bdms,
        int num_habs,
        int num_nodes,
        int num_resources,
        int num_datapoints)
{
    GPtrArray * bdm_list = g_ptr_array_new();

    bionet_bdm_t * bdm;
    bionet_hab_t * hab;
    bionet_node_t * node;
    bionet_resource_t * resource;
    //bionet_datapoint_t * datapoint;
    //bionet_event_t * event;

    int bi, hi, ni, ri;

    for(bi=0; bi<num_bdms; bi++){
        bdm = bionet_bdm_new(bdm_names[bi % num_bdm_names]);
        g_ptr_array_add(bdm_list, bdm);

        GPtrArray *hab_list = bionet_bdm_get_hab_list(bdm);

        for(hi=0; hi<num_habs; hi++){
            hab = bionet_hab_new("check", hab_names[hi % num_hab_names]);
            g_ptr_array_add(hab_list, hab);

            for(ni=0; ni<num_nodes; ni++){
                node = bionet_node_new(hab, node_names[ni % num_node_names]);
                bionet_hab_add_node(hab, node);

                for(ri=0; ri<num_resources; ri++){
                    resource = bionet_resource_new(node, 
                            ri % BIONET_RESOURCE_DATA_TYPE_MAX, 
                            ri % BIONET_RESOURCE_FLAVOR_MAX,
                            resource_names[ri % num_resource_names]);
                    bionet_node_add_resource(node, resource);

                }
            }
        }
    }

    return bdm_list;
}


START_TEST (test_bdm_sync_asn) {
    GPtrArray * bdm_list = make_bdm_list(2,2,2,2,2);

    g_log_set_default_handler(g_log_default_handler, NULL);

    md_iter_state_t make_message_state;
    bdm_list_iterator_t iter;

    bdm_sync_metadata_to_asn_setup(bdm_list, &make_message_state, &iter);

    BDM_Sync_Message_t * sync_message;
    while((sync_message = bdm_sync_metadata_to_asn(&iter, &make_message_state)))
    {

	// Encode as ASN
	asn_enc_rval_t asn_r;
        ssize_t bytes = 0;
	asn_r = der_encode(&asn_DEF_BDM_Sync_Message, sync_message, 
            check_count_bytes, &bytes);

        ASN_STRUCT_FREE(asn_DEF_BDM_Sync_Message, sync_message);

	fail_if (bytes > MTU, "Message (%d bytes) too big (>%d)",
                bytes, MTU);
	printf("Message (%ld bytes)\n", bytes);
    }

} END_TEST /* test_bdm_cfg_read_good */


void data_manager_sync(Suite *s)
{
    TCase *tc = tcase_create("data_manager_sync()");
    suite_add_tcase(s, tc);

    tcase_add_test(tc, test_bdm_sync_asn);

    return;
} /* libutil_hab_tests_suite() */


// vim: ts=8 sw=4 sta expandtab
// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// indent-tabs-mode: nil
// End:
