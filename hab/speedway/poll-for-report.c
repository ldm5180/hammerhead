
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
        handle_tag_report_data(pTagReportData);
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

