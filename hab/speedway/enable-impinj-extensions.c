
#if 0

#include <errno.h>
#include <stdio.h>
#include <glib.h>

#include "ltkc.h"
#include "speedway.h"
#include "out_impinj_ltkc.h"

LLRP_tSMessage *            pRspEnableImpinjMsg = NULL;
LLRP_tSIMPINJ_ENABLE_EXTENSIONS_RESPONSE *pRspEnableImpinj;

int enableImpinjExtensions (void)
{
    //Logic for this method is derived from the LTK Programmer's Guide
	//for enabling Impinj Extensions (v10.8.0, p. 50).
	//Syntax is modified to follow the C-version of the toolkit versus the C++
	//version in the programming guide.

	//compose the command message
	LLRP_tSIMPINJ_ENABLE_EXTENSIONS Cmd = {
        .hdr.elementHdr.pType   = &LLRP_tdIMPINJ_ENABLE_EXTENSIONS,
		.hdr.MessageID          = 1234
	};

	//send the message, expect the response of a certain type
    pRspEnableImpinjMsg = transact(&Cmd.hdr);
    
	//transact() returns NULL if something went wrong
	if(NULL == pRspEnableImpinjMsg)
    {
        return -1;
    }
	
	//cast to a LLRP_tSIMPINJ_ENABLE_EXTENSIONS_RESPONSE
    pRspEnableImpinj = (LLRP_tSIMPINJ_ENABLE_EXTENSIONS_RESPONSE *) pRspEnableImpinjMsg;

    /* Check the LLRPStatus parameter */
	
    if(!(0 == checkLLRPStatus(pRspEnableImpinj->pLLRPStatus, "enableImpinjExtensions")))
    {
        freeMessage(pRspEnableImpinjMsg);
        return -1;
    }
	

    return 0;
}

#endif
