
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "hardware-abstractor.h"

#include "ltkc.h"
#include "speedway.h"




static void process_one_TagReportData(LLRP_tSTagReportData *pTagReportData) {
    char aBuf[64];
    const LLRP_tSTypeDescriptor *pType;

    /*
     * Print the EPC. It could be an 96-bit EPC_96 parameter
     * or an variable length EPCData parameter.
     */
    if(pTagReportData->pEPCParameter != NULL) {
        unsigned int n, i;
        char *p = aBuf;
        llrp_u8_t *pValue = NULL;

        pType = pTagReportData->pEPCParameter->elementHdr.pType;

        if(&LLRP_tdEPC_96 == pType) {
            LLRP_tSEPC_96 * pE96;

            pE96 = (LLRP_tSEPC_96 *) pTagReportData->pEPCParameter;
            pValue = pE96->EPC.aValue;
            n = 12u;
        }
        else if(&LLRP_tdEPCData == pType) {
            LLRP_tSEPCData *pEPCData;

            pEPCData = (LLRP_tSEPCData *) pTagReportData->pEPCParameter;
            pValue = pEPCData->EPC.pValue;
            n = (pEPCData->EPC.nBit + 7u) / 8u;
        }

        if(pValue != NULL) {
            for(i = 0; i < n; i++) {
                if(0 < i && i%2 == 0) {
                    *p++ = '-';
                }

                sprintf(p, "%02X", pValue[i]);

                while(*p) p++;
            }
        }
        else {
            strcpy(aBuf, "---unknown-epc-data-type---");
        }
    }
    else {
        strcpy(aBuf, "---missing-epc-data---");
    }

    printf("%-32s", aBuf);

    printf("\n");
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


