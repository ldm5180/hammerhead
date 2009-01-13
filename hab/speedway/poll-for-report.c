
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
            strcat(node_id, "-");
            node_id_index ++;
        }

        sprintf(&node_id[node_id_index], "%02X", pValue[i]);
        node_id_index += 2;
    }

    node_id[node_id_index] = (char)0;

    return node_id;
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

static void process_one_TagReportData(LLRP_tSTagReportData *pTagReportData) {
    bionet_node_t *node;
    char *node_id;

    node_id = get_tag_id(pTagReportData);
    if (node_id == NULL) return;

    node = bionet_hab_get_node_by_id(hab, node_id);
    if (node == NULL) {
        int r;

	node = bionet_node_new(hab, node_id);
        if (node == NULL) {
            g_warning("error making a Node for Tag %s", node_id);
            return;
        }

	r = bionet_hab_add_node(hab, node);
        if (r != 0) {
            g_warning("error adding Node %s to HAB", node_id);
            return;
	}

        // FIXME: add some resources maybe

        hab_report_new_node(node);
    }

    printf("%-32s\n", node_id);
}




static void process_ro_access_report(LLRP_tSRO_ACCESS_REPORT *report) {
    unsigned int nEntry = 0;
    LLRP_tSTagReportData *pTagReportData;

    /*
     * Loop through and count the number of entries
     */
    for(
        pTagReportData = report->listTagReportData;
        NULL != pTagReportData;
        pTagReportData = (LLRP_tSTagReportData *)pTagReportData->hdr.pNextSubParameter
    ) {
        nEntry++;
    }

    printf("INFO: %u tag report entries\n", nEntry);

    /*
     * Loop through again and print each entry.
     */
    for(
        pTagReportData = report->listTagReportData;
        NULL != pTagReportData;
        pTagReportData = (LLRP_tSTagReportData *)pTagReportData->hdr.pNextSubParameter
    ) {
        process_one_TagReportData(pTagReportData);
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

