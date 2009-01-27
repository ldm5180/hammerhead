
//
// Copyright (C) 2008-2009, Gary Grobe and the Regents of the University of Colorado.
//


#include <errno.h>
#include <stdio.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"

/*
 * Delete all ROSpecs using DELETE_ROSPEC message
 *
 * Per the spec, the DELETE_ROSPEC message contains an ROSpecID of 0 to 
 * indicate we want all ROSpecs deleted.
 *
 * The message is
 *
 * <DELETE_ROSPEC MessageID='102'>
 * 	<ROSpecID>0</ROSpecID>
 * </DELETE_ROSPEC>
 */

int deleteAllROSpecs (void) 
{
    LLRP_tSDELETE_ROSPEC *pCmd;
    LLRP_tSMessage *pCmdMsg;
    LLRP_tSMessage *pRspMsg;
    LLRP_tSDELETE_ROSPEC_RESPONSE *pRsp;

    /*
     * Compose the command message
     */
    pCmd = LLRP_DELETE_ROSPEC_construct();
    pCmdMsg = &pCmd->hdr;
    LLRP_Message_setMessageID(pCmdMsg, 102);
    LLRP_DELETE_ROSPEC_setROSpecID(pCmd, 0);

    /*
     * Send the message, expect the response of certain type
     */
    pRspMsg = transact(pCmdMsg);

    freeMessage(pCmdMsg);

    /*
     * transact() returns NULL if something went wrong.
     */
    if(pRspMsg == NULL) {
        return -1;
    }

    /*
     * Cast to a DELETE_ROSPEC_RESPONSE message.
     */
    pRsp = (LLRP_tSDELETE_ROSPEC_RESPONSE *) pRspMsg;

    /*
     * Check the LLRPStatus parameter.
     */
    if(checkLLRPStatus(pRsp->pLLRPStatus, "deleteAllROSpecs") != 0) {
        freeMessage(pRspMsg);

        return -1;
    }

    freeMessage(pRspMsg);

    return 0;
}

