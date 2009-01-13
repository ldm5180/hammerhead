
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"


int poll_for_report() {
	int bDone = 0;
	int retVal = 0;

	/*
	 * Keep receiving messages until done or until something has happened.
	 */
	while (!bDone) {
		LLRP_tSMessage *pMessage;
		const LLRP_tSTypeDescriptor *pType;

		/*
		 * Wait up to 7 seconds for a message, though the report should
		 * occur within 5 seconds.
		 */
		pMessage = recvMessage(7000);

		if (pMessage == NULL) {
			retVal = -2;
			bDone = 1;

			continue;
		}

		/*
		 * What happens here depends on what kind of message was received.
		 * Use the type label (pType) to discriminate message types.
		 */
		pType = pMessage->elementHdr.pType;

		/*
		 * If this is a tag report, then print it.
		 */
		if (&LLRP_tdRO_ACCESS_REPORT == pType) {
			LLRP_tSRO_ACCESS_REPORT *pNtf;

			pNtf = (LLRP_tSRO_ACCESS_REPORT *) pMessage;

			process_ro_access_report(pNtf);
			bDone = 1;
			retVal = 0;
		}

		/*
		 * If this is a reader event ...
		 */
		else if (&LLRP_tdREADER_EVENT_NOTIFICATION == pType) {
			LLRP_tSREADER_EVENT_NOTIFICATION *pNtf;
			LLRP_tSReaderEventNotificationData *pNtfData;

			pNtf = (LLRP_tSREADER_EVENT_NOTIFICATION *) pMessage;

			pNtfData = 
				LLRP_READER_EVENT_NOTIFICATION_getReaderEventNotificationData(
					pNtf);

			if (pNtfData != NULL) {
				handleReaderEventNotification(pNtfData);
			}
			else {
				// Should never happen.
				printf("Warning: READER_EVENT_NOTIFICATION  without data\n");
			}
		}
		else {
			/*
			 * Something unexpected happened.
			 */
			printf("Warning: Ignoring unexpected message during monitor: %s\n",
				pType->pName);
		}

		freeMessage(pMessage);
	}
	

	return retVal;
}


