
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "bionet-data-manager.h"


void handle_sync_metadata_message(client_t *client, BDM_Sync_Metadata_Message_t *message) {
    GPtrArray *bdm_list;
    int bi;

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Sync Metadata Message {");

    bdm_list = g_ptr_array_new();
    for (bi = 0; bi < message->list.count; bi ++) {
        DataManager_t *asn_bdm;
        bionet_bdm_t *bdm;
        int hi;

        asn_bdm = message->list.array[bi];
        bdm = bionet_bdm_new((char*)asn_bdm->id.buf);
        if ( NULL == bdm ) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                  "Failed to get new BDM: %m");	    
            return;
        }

        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "  BDM: %s", bionet_bdm_get_id(bdm));

        g_ptr_array_add(bdm_list, bdm);

        if (db_add_bdm(main_db, bionet_bdm_get_id(bdm))) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                  "handle_sync_metadata_message(): Failed to add BDM %s to DB.",
                  bionet_bdm_get_id(bdm));
        }

        for (hi = 0; hi < asn_bdm->hablist.list.count; hi ++) {
            HardwareAbstractor_t *asn_hab;
            bionet_hab_t *hab;
            int ni;

            asn_hab = asn_bdm->hablist.list.array[hi];

            hab = bionet_hab_new((char *)asn_hab->type.buf, (char *)asn_hab->id.buf);
            if (hab == NULL) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                      "Failed to get new HAB: %m");	    
                return;
            }
            bionet_hab_set_recording_bdm(hab, bionet_bdm_get_id(bdm));
            
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "    HAB: %s", bionet_hab_get_name(hab));

            bionet_bdm_add_hab(bdm, hab);

            if (db_add_hab(main_db, hab)) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                      "handle_sync_metadata_message(): Failed to add HAB %s to DB.",
                      bionet_hab_get_name(hab));
            }

            for (ni = 0; ni < asn_hab->nodes.list.count; ni ++) {
                Node_t *asn_node;
                bionet_node_t *node;
                int ri;

                asn_node = asn_hab->nodes.list.array[ni];

                node = bionet_node_new(hab, (char *)asn_node->id.buf);
                if (node == NULL) {
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                          "Failed to get new node: %m");
                    return;
                }

                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "        Node: %s", bionet_node_get_id(node));

                if (bionet_hab_add_node(hab, node)) {
                    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                          "handle_Resource_Datapoints_Reply(): Failed to add node to hab.");
                }

                for (ri = 0; ri < asn_node->resources.list.count; ri ++) {
                    Resource_t *asn_resource;
                    bionet_resource_data_type_t datatype;
                    bionet_resource_flavor_t flavor;
                    bionet_resource_t *resource;
                    int di;

                    asn_resource = asn_node->resources.list.array[ri];

                    datatype = bionet_asn_to_datatype(asn_resource->datatype);
                    if (datatype == -1) {
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                              "Unknown datatype.");
                    }

                    flavor = bionet_asn_to_flavor(asn_resource->flavor);
                    if (flavor == -1) {
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                              "Unknown flavor.");
                    }

                    resource = bionet_resource_new(node, datatype, flavor, (char *)asn_resource->id.buf);
                    if (resource == NULL) {
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                              "Failed to get new resource: %m");
                    }

                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "            Resource: %s", bionet_resource_get_id(resource));
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "                datatype: %s", 
                          bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)));
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "                flavor: %s", 
                          bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)));

                    if (bionet_node_add_resource(node, resource)) {
                        g_log("", G_LOG_LEVEL_WARNING, 
                              "handle_Resource_Datapoints_Reply(): Failed to add resource to node.");
                        bionet_resource_free(resource);
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                              "Failed to add resource to node: %m");
                    }

                    for (di = 0; di < asn_resource->datapoints.list.count; di++) {
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
                              "Datapoints should never be in a sync metadata message. %d found.", 
                              asn_resource->datapoints.list.count);
                    }
                }

                if (db_add_node(main_db, node)) {
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                          "handle_sync_metadata_message(): Failed to add node %s to DB.",
                          bionet_node_get_name(node));
                }

            }
        }
    }

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "} Sync Metadata Message");    

    //cleanup
    for (bi = bdm_list->len - 1; bi >= 0; bi--) {
	bionet_bdm_free(g_ptr_array_index(bdm_list, bi));
	g_ptr_array_remove_index(bdm_list, bi);
    }
    g_ptr_array_free(bdm_list, TRUE);
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
