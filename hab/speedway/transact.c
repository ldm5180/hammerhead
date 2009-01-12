#include <errno.h>
#include <glib.h>

#include "ltkc.h"
#include "speedway.h"

/*
 * Wrapper routine to do an LLRP transaction
 *
 * Wrapper to transact a request/resposne.
 *     - Print the outbound message in XML if verbose level is at least 2
 *     - Send it using the LLRP_Conn_transact()
 *     - LLRP_Conn_transact() receives the response or recognizes an error
 *     - Tattle on errors, if any
 *     - Print the received message in XML if verbose level is at least 2
 *     - If the response is ERROR_MESSAGE, the request was sufficiently
 *       misunderstood that the reader could not send a proper reply.
 *       Deem this an error, free the message.
 *
 * The message returned resides in allocated memory. It is the
 * caller's obligtation to free it.
 */

LLRP_tSMessage *transact (LLRP_tSMessage *pSendMsg) 
{
    LLRP_tSConnection *pConn = g_pConnectionToReader;
    LLRP_tSMessage *pRspMsg;

    /*
     * Print the XML text for the outbound message if
     * verbosity is 2 or higher.
     */
    if(g_verbose > 1) {
        printf("\n===================================\n");
        printf("INFO: Transact sending\n");
        printXMLMessage(pSendMsg);
    }

    /*
     * Send the message, expect the response of certain type.
     * If LLRP_Conn_transact() returns NULL then there was
     * an error. In that case we try to print the error details.
     */
    pRspMsg = LLRP_Conn_transact(pConn, pSendMsg, 5000);

    if(pRspMsg == NULL) {
        const LLRP_tSErrorDetails *pError = LLRP_Conn_getTransactError(pConn);

        printf("ERROR: %s transact failed, %s\n",
            pSendMsg->elementHdr.pType->pName,
            pError->pWhatStr ? pError->pWhatStr : "no reason given");

        if(NULL != pError->pRefType) {
            printf("ERROR: ... reference type %s\n",
                pError->pRefType->pName);
        }

        if(NULL != pError->pRefField) {
            printf("ERROR: ... reference field %s\n",
				pError->pRefField->pName);
        }

        return NULL;
    }

    /*
     * Print the XML text for the inbound message if
     * verbosity is 2 or higher.
     */

    if(g_verbose > 1) {
        printf("\n- - - - - - - - - - - - - - - - - -\n");
        printf("INFO: Transact received response\n");
        printXMLMessage(pRspMsg);
    }

    /*
     * If it is an ERROR_MESSAGE (response from reader when it can't 
	 * understand the request), tattle and declare defeat.
     */
    if(&LLRP_tdERROR_MESSAGE == pRspMsg->elementHdr.pType) {
        const LLRP_tSTypeDescriptor *pResponseType;

        pResponseType = pSendMsg->elementHdr.pType->pResponseType;

        printf("ERROR: Received ERROR_MESSAGE instead of %s\n",
            pResponseType->pName);

        freeMessage(pRspMsg);

        pRspMsg = NULL;
    }

    return pRspMsg;
}


