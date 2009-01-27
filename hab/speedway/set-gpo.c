
//
// Copyright (C) 2008-2009, Gary Grobe and the Regents of the University of Colorado.
//


#include <errno.h>
#include <stdio.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"


int set_gpo(int gpo_num, int state) {

    LLRP_tSGPOWriteData gpo_write_data[] = {
        { 
            .hdr.elementHdr.pType = &LLRP_tdGPOWriteData,
            .GPOPortNumber = gpo_num,
            .GPOData = state
        }
    };

    LLRP_tSSET_READER_CONFIG set_reader_config = {
        .hdr.elementHdr.pType   = &LLRP_tdSET_READER_CONFIG,
        .ResetToFactoryDefault = 0,
        .pReaderEventNotificationSpec = NULL,
        .listAntennaProperties = NULL,
        .listAntennaConfiguration = NULL,
        .pROReportSpec = NULL,
        .pAccessReportSpec = NULL,
        .pKeepaliveSpec = NULL,
        .listGPOWriteData = gpo_write_data,
        .listGPIPortCurrentState = NULL,
        .pEventsAndReports = NULL,
        .listCustom = NULL
    };

    LLRP_tSMessage *pRspMsg;
    LLRP_tSSET_READER_CONFIG_RESPONSE *pRsp;


    // Send the message, expect the response of certain type
    pRspMsg = transact(&set_reader_config.hdr);
    if(pRspMsg == NULL) {
        g_warning("error with SetReaderConfig transaction to set GPO %d to %d", gpo_num, state);
        return -1;
    }

    // Check the LLRPStatus parameter
    pRsp = (LLRP_tSSET_READER_CONFIG_RESPONSE *)pRspMsg;
    if(checkLLRPStatus(pRsp->pLLRPStatus, "addROSpec") != 0) {
        g_warning("error with SetReaderConfig transaction to set GPO %d to %d", gpo_num, state);
        freeMessage(pRspMsg);
        return -1;
    }

    freeMessage(pRspMsg);

    return 0;
}

