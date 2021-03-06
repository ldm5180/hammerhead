
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

#include "ltkc.h"
#include "speedway.h"

/*
 * Handle the payload of a READER_EVENT_NOTIFICATION message. This routine
 * dispatches to handlers of specific event types.
 */

void handleReaderEventNotification(LLRP_tSReaderEventNotificationData *pNtfData) {
    int nReported = 0;
    LLRP_tSAntennaEvent *pAntennaEvent;
    LLRP_tSReaderExceptionEvent *pReaderExceptionEvent;
    LLRP_tSGPIEvent *pGPIEvent;

    pAntennaEvent = LLRP_ReaderEventNotificationData_getAntennaEvent(pNtfData);
    if (pAntennaEvent != NULL) {
        nReported++;
    }

    pReaderExceptionEvent = LLRP_ReaderEventNotificationData_getReaderExceptionEvent(pNtfData);
    if (pReaderExceptionEvent != NULL) {
        handleReaderExceptionEvent(pReaderExceptionEvent);
        nReported++;
    }

    pGPIEvent = LLRP_ReaderEventNotificationData_getGPIEvent(pNtfData);
    if (pGPIEvent != NULL) {
        handle_gpi_event(pGPIEvent);
        nReported++;
    }

    /*
     * Similarly handle other events here:
     *      HoppingEvent
     *      ROSpecEvent
     *      ReportBufferLevelWarningEvent
     *      ReportBufferOverflowErrorEvent
     *      RFSurveyEvent
     *      AISpecEvent
     *      ConnectionAttemptEvent
     *      ConnectionCloseEvent
     *      Custom
     */

    if(nReported == 0) {
        g_warning("Unexpected (unhandled) ReaderEvent");
    }
}

