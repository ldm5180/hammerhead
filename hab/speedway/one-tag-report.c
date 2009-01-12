
#include <errno.h>
#include <stdio.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"

/*
 *
 * Helper routine to print one tag report entry on one line
 */

void printOneTagReportData(LLRP_tSTagReportData *pTagReportData)
{
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


