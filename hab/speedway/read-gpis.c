
//
// Copyright (C) 2008-2009, Gary Grobe and the Regents of the University of Colorado.
//


#include <errno.h>
#include <stdio.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"


int read_gpis(int gpi[4]) {

    LLRP_tSGET_READER_CONFIG get_reader_config = {
        .hdr.elementHdr.pType   = &LLRP_tdGET_READER_CONFIG,
        .eRequestedData = LLRP_GetReaderConfigRequestedData_GPIPortCurrentState,
        .GPIPortNum = 0,
        .listCustom = NULL
    };

    LLRP_tSMessage *pRspMsg;
    LLRP_tSGET_READER_CONFIG_RESPONSE *pRsp;

    LLRP_tSGPIPortCurrentState *pGPIPortCurrentState;


    // Send the message, expect the response of certain type
    pRspMsg = transact(&get_reader_config.hdr);
    if(pRspMsg == NULL) {
        g_warning("error with GetReaderConfig transaction to get GPIs");
        return -1;
    }

    // Check the LLRPStatus parameter
    pRsp = (LLRP_tSGET_READER_CONFIG_RESPONSE *)pRspMsg;
    if(checkLLRPStatus(pRsp->pLLRPStatus, "addROSpec") != 0) {
        g_warning("unexpected response in GetReaderConfig transaction to get GPIs");
        freeMessage(pRspMsg);
        return -1;
    }


    // handle each GPIPortCurrentState entry separately
    for(
        pGPIPortCurrentState = pRsp->listGPIPortCurrentState;
        NULL != pGPIPortCurrentState;
        pGPIPortCurrentState = (LLRP_tSGPIPortCurrentState *)pGPIPortCurrentState->hdr.pNextSubParameter
    ) {
        int port_num, config, state;

        port_num = pGPIPortCurrentState->GPIPortNum;
        config = pGPIPortCurrentState->Config;
        state = pGPIPortCurrentState->eState;

        port_num --;

        gpi[port_num] = state;
    }
    

    freeMessage(pRspMsg);

    return 0;
}

