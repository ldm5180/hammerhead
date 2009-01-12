#include <errno.h>
#include <string.h>
#include <glib.h>

#include "ltkc.h"
#include "speedway.h"

/*
 * This can receive notifications as well as responses.
 *	- Recv a message using the LLRP_Conn_recvMessage().
 *  - Tattle on errors, if any.
 *  - Print the message in XML if verbose level is at least 2.
 */

LLRP_tSMessage * recvMessage (int nMaxMS) {
	LLRP_tSConnection *pConn = g_pConnectionToReader;
    LLRP_tSMessage *pMessage;

    /*
     * Receive the message subject to a time limit.
     */
    pMessage = LLRP_Conn_recvMessage(pConn, nMaxMS);

    /*
     * If LLRP_Conn_recvMessage() returns NULL then there was
     * an error. In that case we try to print the error details.
     */
    if(pMessage == NULL) {
        const LLRP_tSErrorDetails *pError = LLRP_Conn_getRecvError(pConn);

        printf("Error: recvMessage failed, %s\n",
            pError->pWhatStr ? pError->pWhatStr : "no reason given");

        if(pError->pRefType != NULL) {
            printf("ERROR: ... reference type %s\n", pError->pRefType->pName);
        }

        if(pError->pRefField != NULL) {
            printf("ERROR: ... reference field %s\n", pError->pRefField->pName);
        }

        return NULL;
    }

    /*
     * Print the XML text for the inbound message if
     * verbosity is 2 or higher.
     */
    if(1 < g_verbose)
    {
        printf("\n===================================\n");
        printf("INFO: Message received\n");
        printXMLMessage(pMessage);
    }

    return pMessage;
}

