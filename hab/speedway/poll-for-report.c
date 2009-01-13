
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
        bionet_resource_t *resource;
        char *rid;

        rid = "Antenna-1";
        resource = bionet_node_get_resource_by_id(node, rid);
        if (resource == NULL) {
            g_warning("error getting Resource %s:%s", bionet_node_get_id(node), rid);
            return;
        }
        bionet_resource_set_binary(resource, 0, NULL);

        rid = "Antenna-2";
        resource = bionet_node_get_resource_by_id(node, rid);
        if (resource == NULL) {
            g_warning("error getting Resource %s:%s", bionet_node_get_id(node), rid);
            return;
        }
        bionet_resource_set_binary(resource, 0, NULL);

        rid = "Antenna-3";
        resource = bionet_node_get_resource_by_id(node, rid);
        if (resource == NULL) {
            g_warning("error getting Resource %s:%s", bionet_node_get_id(node), rid);
            return;
        }
        bionet_resource_set_binary(resource, 0, NULL);

        rid = "Antenna-4";
        resource = bionet_node_get_resource_by_id(node, rid);
        if (resource == NULL) {
            g_warning("error getting Resource %s:%s", bionet_node_get_id(node), rid);
            return;
        }
        bionet_resource_set_binary(resource, 0, NULL);
    }


    // handle each TagReportData entry separately
    for(
        pTagReportData = report->listTagReportData;
        NULL != pTagReportData;
        pTagReportData = (LLRP_tSTagReportData *)pTagReportData->hdr.pNextSubParameter
    ) {
        handle_tag_report_data(pTagReportData);
    }


    // report all the updated resources to Bionet
    for (ni = 0; ni < bionet_hab_get_num_nodes(hab); ni ++) {
        bionet_node_t *node = bionet_hab_get_node_by_index(hab, ni);
        hab_report_datapoints(node);
    }
}




void poll_for_report() {
    LLRP_tSMessage *pMessage = NULL;
    const LLRP_tSTypeDescriptor *pType;

    // wait a little bit for a message
    //
    // FIXME: it'd be nice if the LLRP library exposed a file descriptor so
    // we could roll it into select in our main loop
    pMessage = recvMessage(RECEIVE_TIMEOUT);
    if (pMessage == NULL) {
        // timeout
        return;
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
        goto cleanup;
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
            printf("Warning: READER_EVENT_NOTIFICATION  without data\n");
        }
    } else {
        /*
         * Something unexpected happened.
         */
        printf("Warning: Ignoring unexpected message during monitor: %s\n", pType->pName);
    }

cleanup:
    if (pMessage != NULL) freeMessage(pMessage);
}

