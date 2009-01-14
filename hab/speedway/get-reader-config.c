
#include <errno.h>
#include <stdio.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"


void get_reader_config(void) {
    LLRP_tSGET_READER_CONFIG GetReaderConfig = {
        .hdr.elementHdr.pType = &LLRP_tdGET_READER_CONFIG,
        .eRequestedData = LLRP_GetReaderConfigRequestedData_All,
        .AntennaID = 0,
        .GPIPortNum = 0,
        .GPOPortNum = 0,
        .listCustom = NULL
    };

    LLRP_tSMessage *pRspMsg;
    LLRP_tSGET_READER_CONFIG_RESPONSE *pRsp;

    // Send the message, expect the response of certain type
    pRspMsg = transact(&GetReaderConfig.hdr);
    if(pRspMsg == NULL) {
        g_warning("error getting reader config");
        return;
    }

    // Cast to a ENABLE_ROSPEC_RESPONSE message
    pRsp = (LLRP_tSGET_READER_CONFIG_RESPONSE *)pRspMsg;

    // Check the LLRPStatus parameter
    if(checkLLRPStatus(pRsp->pLLRPStatus, "enableROSpec") != 0) {
        freeMessage(pRspMsg);
        return;
    }

    // Done with the response message
    freeMessage(pRspMsg);

    return;
}

