
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


#define RECEIVE_TIMEOUT 100


static void process_ro_access_report(LLRP_tSRO_ACCESS_REPORT *report) {
    LLRP_tSTagReportData *pTagReportData;
    int ni;


    // set all nodes to "not seen on any antenna"
    for (ni = 0; ni < bionet_hab_get_num_nodes(hab); ni ++) {
        bionet_node_t *node = bionet_hab_get_node_by_index(hab, ni);
        node_data_t *node_data = bionet_node_get_user_data(node);
        int i;

        if (node == reader_node) continue;

        node_data->still_here = 0;

        for (i = 1; i <= 4; i ++) {
            char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];
            bionet_resource_t *resource;

            snprintf(resource_id, sizeof(resource_id), "Antenna-%d", i);
            resource = bionet_node_get_resource_by_id(node, resource_id);
            if (resource == NULL) {
                g_warning("error getting Resource %s:%s", bionet_node_get_id(node), resource_id);
                return;
            }
            bionet_resource_set_binary(resource, 0, NULL);
        }
    }


    // handle each TagReportData entry separately
    // any reports of an antenna seeing a node update that antenna's resources on the node and set the still-here flag
    for(
        pTagReportData = report->listTagReportData;
        NULL != pTagReportData;
        pTagReportData = (LLRP_tSTagReportData *)pTagReportData->hdr.pNextSubParameter
    ) {
        handle_tag_report_data(pTagReportData);
    }


    // report changes to the nodes (gone nodes, node with new datapoints
    for (ni = 0; ni < bionet_hab_get_num_nodes(hab); ni ++) {
        bionet_node_t *node = bionet_hab_get_node_by_index(hab, ni);
        node_data_t *node_data = bionet_node_get_user_data(node);

        if (node == reader_node) continue;

        if (node_data->still_here) {
            hab_report_datapoints(node);
        } else {
            free(node_data);
            bionet_node_set_user_data(node, NULL);
            bionet_hab_remove_node_by_id(hab, bionet_node_get_id(node));
            hab_report_lost_node(node);
            bionet_node_free(node);
            ni --;
        }
    }
}




int poll_reader() {
    LLRP_tSMessage *pMessage = NULL;
    const LLRP_tSTypeDescriptor *pType;

    // wait a little bit for a message
    //
    // FIXME: it'd be nice if the LLRP library exposed a file descriptor so
    // we could roll it into select in our main loop
    pMessage = recvMessage(RECEIVE_TIMEOUT);
    if (pMessage == NULL) {
        // timeout
        return 1;
    }


    /*
     * What happens here depends on what kind of message was received.
     * Use the type label (pType) to discriminate message types.
     */
    pType = pMessage->elementHdr.pType;

    /*
     * If this is a tag report, then process it.
     */
    if (&LLRP_tdRO_ACCESS_REPORT == pType) {
        process_ro_access_report((LLRP_tSRO_ACCESS_REPORT *)pMessage);
        freeMessage(pMessage);

        scans_left_to_do --;
        if (scans_left_to_do > 0) startROSpec();
        return 1;
    }

    /*
     * If this is a reader event ...
     */
    else if (&LLRP_tdREADER_EVENT_NOTIFICATION == pType) {
        LLRP_tSREADER_EVENT_NOTIFICATION *pNtf;
        LLRP_tSReaderEventNotificationData *pNtfData;

        pNtf = (LLRP_tSREADER_EVENT_NOTIFICATION *) pMessage;

        pNtfData = LLRP_READER_EVENT_NOTIFICATION_getReaderEventNotificationData(pNtf);

        if (pNtfData != NULL) {
            handleReaderEventNotification(pNtfData);
        } else {
            // Should never happen.
            g_warning("READER_EVENT_NOTIFICATION  without data");
        }
    } else {
        /*
         * Something unexpected happened.
         */
        g_warning("Ignoring unexpected message during monitor: %s", pType->pName);
    }

    freeMessage(pMessage);

    return 1;
}

