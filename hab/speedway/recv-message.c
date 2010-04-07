
// Copyright (c) 2008-2010, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
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

        if (strcmp(pError->pWhatStr, "timeout") == 0) {
            // timeouts are ok, since the LLRP library forces us to poll for messages, grump
            return NULL;
        }

        g_warning("recvMessage failed, %s", pError->pWhatStr ? pError->pWhatStr : "no reason given");

        if(pError->pRefType != NULL) {
            g_warning("... reference type %s", pError->pRefType->pName);
        }

        if(pError->pRefField != NULL) {
            g_warning("... reference field %s", pError->pRefField->pName);
        }

        return NULL;
    }

    /*
     * Print the XML text for the inbound message if
     * verbosity is 2 or higher.
     */
    if(show_messages) {
        g_message("===================================");
        g_message("Message received");
        printXMLMessage(pMessage);
    }

    return pMessage;
}

