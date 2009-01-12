
#include <errno.h>
#include <stdio.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"

/*
 * Helper routine to check an LLRPStatus parameter and tattle on errors.
 *
 * Helper routine to interpret the LLRPStatus subparameter that is in all 
 * responses. It tattles on an error, if one, and tries to safely provide 
 * details.
 *
 */ 

int checkLLRPStatus(LLRP_tSLLRPStatus *pLLRPStatus, char *pWhatStr)
{
    /*
     * The LLRPStatus parameter is mandatory in all responses.
     * If it is missing there should have been a decode error.
     * This just makes sure (remember, this program is a
     * diagnostic and suppose to catch LTKC mistakes).
     */
    if(pLLRPStatus == NULL) {
        printf("ERROR: %s missing LLRP status\n", pWhatStr);

        return -1;
    }

    /*
     * Make sure the status is M_Success. If it isn't, print the error string 
	 * if one. This does not try to pretty-print the status code. To get 
	 * that, run this program with -vv and examine the XML output.
     */
    if(LLRP_StatusCode_M_Success != pLLRPStatus->eStatusCode) {
        if(pLLRPStatus->ErrorDescription.nValue == 0) {
            printf("ERROR: %s failed, no error description given\n",
                pWhatStr);
        }
        else {
            printf("ERROR: %s failed, %.*s\n", 
				pWhatStr,
                pLLRPStatus->ErrorDescription.nValue,
                pLLRPStatus->ErrorDescription.pValue);
        }

        return -2;
    }
   
	return 0;
}

