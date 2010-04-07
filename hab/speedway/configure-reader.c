
// Copyright (c) 2008-2010, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"

extern LLRP_tSGET_READER_CAPABILITIES_RESPONSE *pRspCapabilities;

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

    LLRP_tSReceiveSensitivityTableEntry * cur_table_entry =
	pRspCapabilities->pGeneralDeviceCapabilities->listReceiveSensitivityTableEntry;
    llrp_u16_t rf_sensitivity_index = 0;
    while (cur_table_entry) {
	if (cur_table_entry->ReceiveSensitivityValue == rf_sensitivity) {
	    rf_sensitivity_index = cur_table_entry->Index;
	    break;
	}
	cur_table_entry = LLRP_GeneralDeviceCapabilities_nextReceiveSensitivityTableEntry(cur_table_entry); 
    }

    LLRP_tSRFReceiver rfreceiver_sensitivity = {
	.hdr.elementHdr.pType = &LLRP_tdRFReceiver,
	.ReceiverSensitivity = rf_sensitivity_index
    };

    LLRP_tSAntennaConfiguration antenna_config = {
	    .hdr.elementHdr.pType = &LLRP_tdAntennaConfiguration,
	    .AntennaID = 0,
	    .pRFReceiver = &rfreceiver_sensitivity,
	    .pRFTransmitter = NULL,
	    .listAirProtocolInventoryCommandSettings = NULL
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
        .listAntennaConfiguration = &antenna_config,
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

