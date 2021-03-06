
// Copyright (c) 2008-2010, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.



#include <errno.h>
#include <stdio.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"

/*
 * Check the connection status message from the reader.
 *
 * We are expecting a READER_EVENT_NOTIFICATION message that tells us the
 * connection is OK. The reader is suppose to send the message promptly upon
 * connection.
 *
 * If there is already another LLRP connection to the reader we'll get a bad 
 * status. 
 *
 * The message should be something like:
 * 	<READER_EVENT_NOTIFICATION MessageID='0'>
 * 		<ReaderEventNotificationData>
 * 			<UTCTimestamp>
 * 				<Microseconds>1184491439614224</Microseconds>
 * 			</UTCTimestamp>
 * 			<ConnectionAttempEvent>
 * 				<Status>Success</Status>
 * 			</ConnectionAttempEvent>
 * 		</ReaderEventNotificationData>
 * 	</READER_EVENT_NOTIFICATION>
 */

int checkConnectionStatus(void) 
{
    LLRP_tSMessage *pMessage;
    LLRP_tSREADER_EVENT_NOTIFICATION *pNtf;
    LLRP_tSReaderEventNotificationData *pNtfData;
    LLRP_tSConnectionAttemptEvent *pEvent = NULL;

    /*
     * Expect the notification within 10 seconds.
     * It is suppose to be the very first message sent.
     */
    pMessage = recvMessage(10000);

    /*
     * recvMessage() returns NULL if something went wrong.
     */
    if(pMessage == NULL) {
        goto fail2;
    }

    /*
     * Check to make sure the message is of the right type.
     * The type label (pointer) in the message should be
     * the type descriptor for READER_EVENT_NOTIFICATION.
     */
    if(&LLRP_tdREADER_EVENT_NOTIFICATION != pMessage->elementHdr.pType) {
        goto fail2;
    }

    /*
     * Now that we are sure it is a READER_EVENT_NOTIFICATION,
     * traverse to the ReaderEventNotificationData parameter.
     */
    pNtf = (LLRP_tSREADER_EVENT_NOTIFICATION *) pMessage;
    pNtfData = pNtf->pReaderEventNotificationData;

    if(pNtfData == NULL) {
        goto fail2;
    }

    /*
     * The ConnectionAttemptEvent parameter must be present.
     */
    pEvent = pNtfData->pConnectionAttemptEvent;

    if(pEvent == NULL) {
        goto fail2;
    }

    /*
     * The status in the ConnectionAttemptEvent parameter
     * must indicate connection success.
     */
    if(LLRP_ConnectionAttemptStatusType_Success != pEvent->eStatus) {
        goto fail;
    }

    /*
     * Done with the message
     */
    freeMessage(pMessage);

    return 0;

fail:
    g_warning("checkConnectionStatus failed");

    switch (pEvent->eStatus) {
			
 	case 0:	
	    g_warning("Connection Attempt Success");
	    break;
			
	case 1:
	    g_warning("Connection Attempt Failed a Reader Initiated Connection Already Exists");
	    break;
			
	case 2:
	    g_warning("Connection Attempt Failed a Client Initiated Connection Already Exists");
	    break;
			
	case 3:
	    g_warning("Connection Attempt Failed Reason Other Than a Connection Already Exists");
	    break;
			
	case 4:
	    g_warning("Connection Attempt Another Connection Attempted");
	    break;
			
	default:
	    break;
    }

fail2:
    freeMessage(pMessage);

    return -1;
}


