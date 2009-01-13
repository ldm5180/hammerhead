
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

void handleReaderEventNotification(LLRP_tSReaderEventNotificationData 
		*pNtfData) 
{
    int nReported = 0;
    LLRP_tSAntennaEvent *pAntennaEvent;
    LLRP_tSReaderExceptionEvent *pReaderExceptionEvent;

    pAntennaEvent = 
		LLRP_ReaderEventNotificationData_getAntennaEvent(pNtfData);

    if(pAntennaEvent != NULL) {
        handleAntennaEvent(pAntennaEvent);
        nReported++;
    }

    pReaderExceptionEvent =
        LLRP_ReaderEventNotificationData_getReaderExceptionEvent(pNtfData);

    if(pReaderExceptionEvent != NULL) {
        handleReaderExceptionEvent(pReaderExceptionEvent);
        nReported++;
    }

    /*
     * Similarly handle other events here:
     *      HoppingEvent
     *      GPIEvent
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

