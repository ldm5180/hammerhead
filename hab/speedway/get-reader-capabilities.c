
// Copyright (c) 2008-2009, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"

LLRP_tSMessage *            pRspCapabilitiesMsg = NULL;
LLRP_tSGET_READER_CAPABILITIES_RESPONSE *pRspCapabilities;

int get_reader_capabilities (void)
{
    LLRP_tSGET_READER_CAPABILITIES Cmd = {
        .hdr.elementHdr.pType   = &LLRP_tdGET_READER_CAPABILITIES,
        .hdr.MessageID          = 120,

        .eRequestedData         = LLRP_GetReaderCapabilitiesRequestedData_All,
    };

    if (pRspCapabilitiesMsg) {
	free(pRspCapabilitiesMsg);
	pRspCapabilitiesMsg = NULL;
	pRspCapabilities = NULL;
    }

    pRspCapabilitiesMsg = transact(&Cmd.hdr);
    if(NULL == pRspCapabilitiesMsg)
    {
        return -1;
    }

    pRspCapabilities = (LLRP_tSGET_READER_CAPABILITIES_RESPONSE *) pRspCapabilitiesMsg;

    /* Check the LLRPStatus parameter */
    if(0 != checkLLRPStatus(pRspCapabilities->pLLRPStatus, "getAllCapabilities"))
    {
        freeMessage(pRspCapabilitiesMsg);
        return -1;
    }

    return 0;
}

