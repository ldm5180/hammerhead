
// Copyright (c) 2008-2009, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"


//
// This function configures the reader for the JSC "Trashcan" and "Portal" demos.
//
// enable GPI1, and enable GPI Event Notification
//

int configure_reader(void) {

    // enable all the GPI pins
    LLRP_tSGPIPortCurrentState gpi_port_state[] = {
        { 
            .hdr.elementHdr.pType = &LLRP_tdGPIPortCurrentState,
            .GPIPortNum = 1,
            .Config = 1  // enabled
        },

        { 
            .hdr.elementHdr.pType = &LLRP_tdGPIPortCurrentState,
            .GPIPortNum = 2,
            .Config = 1  // enabled
        },

        { 
            .hdr.elementHdr.pType = &LLRP_tdGPIPortCurrentState,
            .GPIPortNum = 3,
            .Config = 1  // enabled
        },

        { 
            .hdr.elementHdr.pType = &LLRP_tdGPIPortCurrentState,
            .GPIPortNum = 4,
            .Config = 1  // enabled
        }
    };

    LLRP_tSEventNotificationState notifications[] = {
        {
            .hdr.elementHdr.pType = &LLRP_tdEventNotificationState,
            .eEventType = LLRP_NotificationEventType_GPI_Event,
            .NotificationState = 1  // enabled
        }
    };

    LLRP_tSReaderEventNotificationSpec events = {
        .hdr.elementHdr.pType = &LLRP_tdReaderEventNotificationSpec,
        .listEventNotificationState = notifications
    };

    LLRP_tSSET_READER_CONFIG set_reader_config = {
        .hdr.elementHdr.pType   = &LLRP_tdSET_READER_CONFIG,
        .ResetToFactoryDefault = 0,
        .pReaderEventNotificationSpec = &events,
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

