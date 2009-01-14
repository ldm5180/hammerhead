
#include <errno.h>
#include <stdio.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"


int configure_gpi(void) {

    LLRP_tSGPIPortCurrentState gpi_port_state[] = {
        { 
            .hdr.elementHdr.pType = &LLRP_tdGPIPortCurrentState,
            .GPIPortNum = 1,
            .Config = 1  // enabled
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
        .listGPOWriteData = NULL,
        .listGPIPortCurrentState = gpi_port_state,
        .pEventsAndReports = NULL,
        .listCustom = NULL
    };

    LLRP_tSMessage *pRspMsg;
    LLRP_tSSET_READER_CONFIG_RESPONSE *pRsp;


    // Send the message, expect the response of certain type
    pRspMsg = transact(&set_reader_config.hdr);
    if(pRspMsg == NULL) {
        g_warning("error with SetReaderConfig transaction to configure GPIO");
        return -1;
    }

    // Check the LLRPStatus parameter
    pRsp = (LLRP_tSSET_READER_CONFIG_RESPONSE *)pRspMsg;
    if(checkLLRPStatus(pRsp->pLLRPStatus, "addROSpec") != 0) {
        g_warning("unexpected response in SetReaderConfig transaction to configure GPIO");
        freeMessage(pRspMsg);
        return -1;
    }

    freeMessage(pRspMsg);

    return 0;
}

