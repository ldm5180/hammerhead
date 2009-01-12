#include <errno.h>
#include <glib.h>

#include "ltkc.h"
#include "speedway.h"

/*
 * Prints tag report data.
 */
void printTagReportData (LLRP_tSRO_ACCESS_REPORT *pRO_ACCESS_REPORT)
{
    unsigned int nEntry = 0;
    LLRP_tSTagReportData *pTagReportData;

    /*
     * Loop through and count the number of entries
     */
    for(pTagReportData = pRO_ACCESS_REPORT->listTagReportData;
        NULL != pTagReportData; pTagReportData = (LLRP_tSTagReportData *)
			pTagReportData->hdr.pNextSubParameter) {
        nEntry++;
    }

    printf("INFO: %u tag report entries\n", nEntry);

    /*
     * Loop through again and print each entry.
     */
    for(pTagReportData = pRO_ACCESS_REPORT->listTagReportData;
        NULL != pTagReportData; pTagReportData = (LLRP_tSTagReportData *)
			pTagReportData->hdr.pNextSubParameter) {
        printOneTagReportData(pTagReportData);
    }
}


