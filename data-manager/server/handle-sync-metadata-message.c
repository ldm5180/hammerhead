
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "bionet-data-manager.h"
#include "bdm-util.h"


void handle_sync_metadata_message(client_t *client, BDM_Sync_Metadata_Message_t *message) {
    GPtrArray *hab_list;
    int hi;

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "Sync Metadata Message {");

    hab_list = g_ptr_array_new();
    for (hi = 0; hi < message->list.count; hi ++) {
        HardwareAbstractor_t *asn_hab;
        bionet_hab_t *hab;
        int ni;

        asn_hab = message->list.array[hi];

        hab = bionet_hab_new((char *)asn_hab->type.buf, (char *)asn_hab->id.buf);
        if (hab == NULL) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "Failed to get new HAB: %m");	    
	    return;
	}
	
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "    HAB: %s", bionet_hab_get_name(hab));

        g_ptr_array_add(hab_list, hab);

	if (db_add_hab(hab)) {
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

	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "        Node: %s", bionet_node_get_id(node));

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

		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "            Resource: %s", bionet_resource_get_id(resource));
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "                datatype: %s", 
		      bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)));
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "                flavor: %s", 
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

	    if (db_add_node(node)) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "handle_sync_metadata_message(): Failed to add node %s to DB.",
		      bionet_node_get_name(node));
	    }

        }
    }

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "} Sync Metadata Message");    

    //cleanup
    for (hi = hab_list->len - 1; hi >= 0; hi--) {
	bionet_hab_free(g_ptr_array_index(hab_list, hi));
	g_ptr_array_remove_index(hab_list, hi);
    }
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
