
// Copyright (c) 2008-2010, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "hardware-abstractor.h"

#include "ltkc.h"
#include "speedway.h"




static char *get_tag_id (LLRP_tSTagReportData *pTagReportData) {
    unsigned int num_data_bytes;
    unsigned int node_id_index = 0;
    unsigned int i;
    llrp_u8_t *pValue = NULL;
    const LLRP_tSTypeDescriptor *pType;

    static char node_id[BIONET_NAME_COMPONENT_MAX_LEN];

    if (pTagReportData->pEPCParameter == NULL) {
        g_warning("got a Tag Report with missing EPC Parameter!");
        return NULL;
    }

    pType = pTagReportData->pEPCParameter->elementHdr.pType;
    if(&LLRP_tdEPC_96 == pType) {
        LLRP_tSEPC_96 * pE96;

        pE96 = (LLRP_tSEPC_96 *) pTagReportData->pEPCParameter;
        pValue = pE96->EPC.aValue;
        num_data_bytes = 12u;

    } else if(&LLRP_tdEPCData == pType) {
        LLRP_tSEPCData *pEPCData;

        pEPCData = (LLRP_tSEPCData *) pTagReportData->pEPCParameter;
        pValue = pEPCData->EPC.pValue;
        num_data_bytes = (pEPCData->EPC.nBit + 7u) / 8u;
    }

    if (pValue == NULL) {
        g_warning("got a Tag Report with unknown EPC data type");
        return NULL;
    }

    node_id[0] = (char)0;
    node_id_index = 0;

    for (i = 0; i < num_data_bytes; i++) {
        if ((i > 0) && (i%2 == 0)) {
            strncat(node_id, "-", BIONET_NAME_COMPONENT_MAX_LEN - strlen(node_id) - 1);
            node_id_index ++;
        }

        snprintf(&node_id[node_id_index], sizeof(node_id) - node_id_index, "%02X", pValue[i]);
        node_id_index += 2;
    }

    node_id[node_id_index] = (char)0;

    return node_id;
}




bionet_node_t *make_new_node(const char *node_id) {
    int r;
    bionet_node_t *node;
    bionet_resource_t *resource;
    node_data_t *node_data;
    int i;

    node = bionet_node_new(hab, node_id);
    if (node == NULL) {
        g_warning("error making a Node for Tag %s", node_id);
        return NULL;
    }

    node_data = (node_data_t *)calloc(1, sizeof(node_data_t));
    if (node_data == NULL) {
        g_warning("out of memory");
        bionet_node_free(node);
        return NULL;
    }

    bionet_node_set_user_data(node, node_data);

    for (i = 1; i <= 4; i ++) {
        char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];

        snprintf(resource_id, sizeof(resource_id), "Antenna-%d", i);
        resource = bionet_resource_new(
            node,
            BIONET_RESOURCE_DATA_TYPE_BINARY,
            BIONET_RESOURCE_FLAVOR_SENSOR,
            resource_id
        );
        if (resource == NULL) {
            g_warning("error making a Resource %s:%s", node_id, resource_id);
            return NULL;
        }
        r = bionet_node_add_resource(node, resource);
        if (r != 0) {
            g_warning("error making Resource %s:%s", node_id, resource_id);
            return NULL;
        }

        snprintf(resource_id, sizeof(resource_id), "Antenna-%d-PeakRSSI", i);
        resource = bionet_resource_new(
            node,
            BIONET_RESOURCE_DATA_TYPE_INT8,
            BIONET_RESOURCE_FLAVOR_SENSOR,
            resource_id
        );
        if (resource == NULL) {
            g_warning("error making a Resource %s:%s", node_id, resource_id);
            return NULL;
        }
        r = bionet_node_add_resource(node, resource);
        if (r != 0) {
            g_warning("error making Resource %s:%s", node_id, resource_id);
            return NULL;
        }

    }

    r = bionet_hab_add_node(hab, node);
    if (r != 0) {
        g_warning("error adding Node %s to HAB", node_id);
        return NULL;
    }

    hab_report_new_node(node);

    return node;
}




//
//    The Tag Reports requested by our RO look like this:
//
//    <TagReportData>
//        <EPC_96>
//            <EPC>3F2040298000026000000161</EPC>
//        </EPC_96>
//        <AntennaID>
//            <AntennaID>2</AntennaID>
//        </AntennaID>
//        <PeakRSSI>
//            <PeakRSSI>-51</PeakRSSI>
//        </PeakRSSI>
//    </TagReportData>
//                                                

void handle_tag_report_data(LLRP_tSTagReportData *pTagReportData) {
    char *node_id;
    bionet_node_t *node;
    node_data_t *node_data;
    int antenna;

    char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];
    bionet_resource_t *resource;

    node_id = get_tag_id(pTagReportData);
    if (node_id == NULL) return;

    node = bionet_hab_get_node_by_id(hab, node_id);
    if (node == NULL) {
        node = make_new_node(node_id);
        if (node == NULL) return;
    }

    if (pTagReportData->pAntennaID == NULL) {
        g_warning("no antenna for Tag %s!", node_id);
        return;
    }

    node_data = bionet_node_get_user_data(node);

    antenna = pTagReportData->pAntennaID->AntennaID;

    node_data->still_here = 1;

    snprintf(resource_id, sizeof(resource_id), "Antenna-%d", antenna);
    resource = bionet_node_get_resource_by_id(node, resource_id);
    if (resource == NULL) {
        g_warning("error getting Resource %s:%s", node_id, resource_id);
        return;
    }
    bionet_resource_set_binary(resource, 1, NULL);

    snprintf(resource_id, sizeof(resource_id), "Antenna-%d-PeakRSSI", antenna);
    resource = bionet_node_get_resource_by_id(node, resource_id);
    if (resource == NULL) {
        g_warning("error getting Resource %s:%s", node_id, resource_id);
        return;
    }
    bionet_resource_set_int8(resource, pTagReportData->pPeakRSSI->PeakRSSI, NULL);
}

