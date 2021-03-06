
// Copyright (c) 2008-2010, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"


int addROSpec_Immediate(void) {

    // 
    // The RO start & stop specs
    //

    LLRP_tSROSpecStartTrigger ROSpecStartTrigger = {
        .hdr.elementHdr.pType   = &LLRP_tdROSpecStartTrigger,
        .eROSpecStartTriggerType = LLRP_ROSpecStartTriggerType_Immediate
    };

    LLRP_tSROSpecStopTrigger ROSpecStopTrigger = {
        .hdr.elementHdr.pType   = &LLRP_tdROSpecStopTrigger,
        .eROSpecStopTriggerType = LLRP_ROSpecStopTriggerType_Null,
        .DurationTriggerValue   = 0     
    };

    LLRP_tSROBoundarySpec ROBoundarySpec = {
        .hdr.elementHdr.pType   = &LLRP_tdROBoundarySpec,
        .pROSpecStartTrigger    = &ROSpecStartTrigger,
        .pROSpecStopTrigger     = &ROSpecStopTrigger,
    };


    // 
    // The Reader Operation contains a single Antenna Inventory, saying
    // to scan all antennas.
    //

    llrp_u16_t AntennaIDs[1] = { 0 }; // 0 is all antenna's.

    LLRP_tSTagObservationTrigger TagObservationTrigger = {
        .hdr.elementHdr.pType = &LLRP_tdTagObservationTrigger,
        .eTriggerType         = LLRP_TagObservationTriggerType_Upon_Seeing_No_More_New_Tags_For_Tms_Or_Timeout,
        .T                    = scan_idle,
        .Timeout              = scan_timeout
    };

    LLRP_tSAISpecStopTrigger    AISpecStopTrigger = {
        .hdr.elementHdr.pType   = &LLRP_tdAISpecStopTrigger,
        .eAISpecStopTriggerType = LLRP_AISpecStopTriggerType_Tag_Observation,
        .pTagObservationTrigger = &TagObservationTrigger
    };

    LLRP_tSInventoryParameterSpec InventoryParameterSpec = {
        .hdr.elementHdr.pType   = &LLRP_tdInventoryParameterSpec,
        .InventoryParameterSpecID = 1234,
        .eProtocolID            = LLRP_AirProtocols_EPCGlobalClass1Gen2,
    };

    LLRP_tSAISpec AISpec = {
        .hdr.elementHdr.pType   = &LLRP_tdAISpec,
        .AntennaIDs = {
            .nValue                 = 1, 
            .pValue                 = AntennaIDs
        },
        .pAISpecStopTrigger     = &AISpecStopTrigger,
        .listInventoryParameterSpec = &InventoryParameterSpec,
    };


    // 
    // This specifies the report we want from the Reader each time the RO
    // finishes.
    //

    LLRP_tSTagReportContentSelector TagReportContentSelector = {
        .hdr.elementHdr.pType   = &LLRP_tdTagReportContentSelector,
        .EnableROSpecID         = 0,
        .EnableSpecIndex        = 0,
        .EnableInventoryParameterSpecID = 0,
        .EnableAntennaID        = 1,
        .EnableChannelIndex     = 0,
        .EnablePeakRSSI         = 1,
        .EnableFirstSeenTimestamp = 0,
        .EnableLastSeenTimestamp = 0,
        .EnableTagSeenCount     = 0,
        .EnableAccessSpecID     = 0,
    };

    LLRP_tSROReportSpec ROReportSpec;
    LLRP_tSROReportSpec * pROReportSpec = &ROReportSpec;
    if (simple_report) {
	pROReportSpec = getConfiguredROReportSpec();
    } else {
	ROReportSpec.hdr.elementHdr.pType   = &LLRP_tdROReportSpec;
	ROReportSpec.eROReportTrigger       = LLRP_ROReportTriggerType_Upon_N_Tags_Or_End_Of_ROSpec;
	ROReportSpec.N = 1;
	ROReportSpec.pTagReportContentSelector = &TagReportContentSelector;
    }

    LLRP_tSROSpec ROSpec = {
        .hdr.elementHdr.pType   = &LLRP_tdROSpec,
        .ROSpecID               = 123,
        .Priority               = 0,
        .eCurrentState          = LLRP_ROSpecState_Disabled,
        .pROBoundarySpec        = &ROBoundarySpec,
        .listSpecParameter      = &AISpec.hdr,
        .pROReportSpec          = pROReportSpec,
    };

    LLRP_tSADD_ROSPEC Cmd = {
        .hdr.elementHdr.pType   = &LLRP_tdADD_ROSPEC,
        .hdr.MessageID          = 201,
        .pROSpec                = &ROSpec,
    };

    LLRP_tSMessage *pRspMsg;
    LLRP_tSADD_ROSPEC_RESPONSE *pRsp;


    pRspMsg = transact(&Cmd.hdr);
    if(pRspMsg == NULL) {
        return -1;
    }

    pRsp = (LLRP_tSADD_ROSPEC_RESPONSE *) pRspMsg;
    if(checkLLRPStatus(pRsp->pLLRPStatus, "addROSpec") != 0) {
        freeMessage(pRspMsg);
        return -1;
    }

    freeMessage(pRspMsg);

    return 0;
}


int addROSpec_Null(void) {

    // 
    // The RO start & stop specs
    //
    // this is a Null trigger, so it just starts when we tell it to
    LLRP_tSROSpecStartTrigger ROSpecStartTrigger = {
        .hdr.elementHdr.pType   = &LLRP_tdROSpecStartTrigger,
        .eROSpecStartTriggerType = LLRP_ROSpecStartTriggerType_Null,
    };


    LLRP_tSROSpecStopTrigger ROSpecStopTrigger = {
        .hdr.elementHdr.pType   = &LLRP_tdROSpecStopTrigger,
        .eROSpecStopTriggerType = LLRP_ROSpecStopTriggerType_Null,
        .DurationTriggerValue   = 0     
    };

    LLRP_tSROBoundarySpec ROBoundarySpec = {
        .hdr.elementHdr.pType   = &LLRP_tdROBoundarySpec,
        .pROSpecStartTrigger    = &ROSpecStartTrigger,
        .pROSpecStopTrigger     = &ROSpecStopTrigger,
    };


    // 
    // The Reader Operation contains a single Antenna Inventory, saying
    // to scan all antennas.
    //

    llrp_u16_t AntennaIDs[1] = { 0 }; // 0 is all antenna's.

    LLRP_tSTagObservationTrigger TagObservationTrigger = {
        .hdr.elementHdr.pType = &LLRP_tdTagObservationTrigger,
        .eTriggerType         = LLRP_TagObservationTriggerType_Upon_Seeing_No_More_New_Tags_For_Tms_Or_Timeout,
        .T                    = scan_idle,
        .Timeout              = scan_timeout
    };

    LLRP_tSAISpecStopTrigger    AISpecStopTrigger = {
        .hdr.elementHdr.pType   = &LLRP_tdAISpecStopTrigger,
        .eAISpecStopTriggerType = LLRP_AISpecStopTriggerType_Tag_Observation,
        .pTagObservationTrigger = &TagObservationTrigger
    };

    LLRP_tSInventoryParameterSpec InventoryParameterSpec = {
        .hdr.elementHdr.pType   = &LLRP_tdInventoryParameterSpec,
        .InventoryParameterSpecID = 1234,
        .eProtocolID            = LLRP_AirProtocols_EPCGlobalClass1Gen2,
    };

    LLRP_tSAISpec AISpec = {
        .hdr.elementHdr.pType   = &LLRP_tdAISpec,
        .AntennaIDs = {
            .nValue                 = 1, 
            .pValue                 = AntennaIDs
        },
        .pAISpecStopTrigger     = &AISpecStopTrigger,
        .listInventoryParameterSpec = &InventoryParameterSpec,
    };


    // 
    // This specifies the report we want from the Reader each time the RO
    // finishes.
    //

    LLRP_tSTagReportContentSelector TagReportContentSelector = {
        .hdr.elementHdr.pType   = &LLRP_tdTagReportContentSelector,
        .EnableROSpecID         = 0,
        .EnableSpecIndex        = 0,
        .EnableInventoryParameterSpecID = 0,
        .EnableAntennaID        = 1,
        .EnableChannelIndex     = 0,
        .EnablePeakRSSI         = 1,
        .EnableFirstSeenTimestamp = 0,
        .EnableLastSeenTimestamp = 0,
        .EnableTagSeenCount     = 0,
        .EnableAccessSpecID     = 0,
    };

    LLRP_tSROReportSpec ROReportSpec;
    LLRP_tSROReportSpec * pROReportSpec = &ROReportSpec;
    if (simple_report) {
	pROReportSpec = getConfiguredROReportSpec();
    } else {
	ROReportSpec.hdr.elementHdr.pType   = &LLRP_tdROReportSpec;
	ROReportSpec.eROReportTrigger       = LLRP_ROReportTriggerType_Upon_N_Tags_Or_End_Of_ROSpec;
	ROReportSpec.N = 1;
	ROReportSpec.pTagReportContentSelector = &TagReportContentSelector;
    }

    LLRP_tSROSpec ROSpec = {
        .hdr.elementHdr.pType   = &LLRP_tdROSpec,
        .ROSpecID               = 123,
        .Priority               = 0,
        .eCurrentState          = LLRP_ROSpecState_Disabled,
        .pROBoundarySpec        = &ROBoundarySpec,
        .listSpecParameter      = &AISpec.hdr,
        .pROReportSpec          = pROReportSpec,
    };

    LLRP_tSADD_ROSPEC Cmd = {
        .hdr.elementHdr.pType   = &LLRP_tdADD_ROSPEC,
        .hdr.MessageID          = 201,
        .pROSpec                = &ROSpec,
    };

    LLRP_tSMessage *pRspMsg;
    LLRP_tSADD_ROSPEC_RESPONSE *pRsp;


    pRspMsg = transact(&Cmd.hdr);
    if(pRspMsg == NULL) {
        return -1;
    }

    pRsp = (LLRP_tSADD_ROSPEC_RESPONSE *) pRspMsg;
    if(checkLLRPStatus(pRsp->pLLRPStatus, "addROSpec") != 0) {
        freeMessage(pRspMsg);
        return -1;
    }

    freeMessage(pRspMsg);

    return 0;
}


int addROSpec_GPI(void) {

    // 
    // The RO start & stop specs
    //
    // this is a GPI trigger, so it starts when we push a button
    LLRP_tSGPITriggerValue GPITriggerValue = {
        .hdr.elementHdr.pType = &LLRP_tdGPITriggerValue,
        .GPIPortNum = 1,
        .GPIEvent = 1,
        .Timeout = 0
    };

    LLRP_tSROSpecStartTrigger ROSpecStartTrigger = {
        .hdr.elementHdr.pType   = &LLRP_tdROSpecStartTrigger,
        .eROSpecStartTriggerType = LLRP_ROSpecStartTriggerType_GPI,
        .pGPITriggerValue = &GPITriggerValue
    };

    LLRP_tSROSpecStopTrigger ROSpecStopTrigger = {
        .hdr.elementHdr.pType   = &LLRP_tdROSpecStopTrigger,
        .eROSpecStopTriggerType = LLRP_ROSpecStopTriggerType_Null,
        .DurationTriggerValue   = 0     
    };

    LLRP_tSROBoundarySpec ROBoundarySpec = {
        .hdr.elementHdr.pType   = &LLRP_tdROBoundarySpec,
        .pROSpecStartTrigger    = &ROSpecStartTrigger,
        .pROSpecStopTrigger     = &ROSpecStopTrigger,
    };


    // 
    // The Reader Operation contains a single Antenna Inventory, saying
    // to scan all antennas.
    //

    llrp_u16_t AntennaIDs[1] = { 0 }; // 0 is all antenna's.

    LLRP_tSTagObservationTrigger TagObservationTrigger = {
        .hdr.elementHdr.pType = &LLRP_tdTagObservationTrigger,
        .eTriggerType         = LLRP_TagObservationTriggerType_Upon_Seeing_No_More_New_Tags_For_Tms_Or_Timeout,
        .T                    = scan_idle,
        .Timeout              = scan_timeout
    };

    LLRP_tSAISpecStopTrigger    AISpecStopTrigger = {
        .hdr.elementHdr.pType   = &LLRP_tdAISpecStopTrigger,
        .eAISpecStopTriggerType = LLRP_AISpecStopTriggerType_Tag_Observation,
        .pTagObservationTrigger = &TagObservationTrigger
    };

    LLRP_tSInventoryParameterSpec InventoryParameterSpec = {
        .hdr.elementHdr.pType   = &LLRP_tdInventoryParameterSpec,
        .InventoryParameterSpecID = 1234,
        .eProtocolID            = LLRP_AirProtocols_EPCGlobalClass1Gen2,
    };

    LLRP_tSAISpec AISpec = {
        .hdr.elementHdr.pType   = &LLRP_tdAISpec,
        .AntennaIDs = {
            .nValue                 = 1, 
            .pValue                 = AntennaIDs
        },
        .pAISpecStopTrigger     = &AISpecStopTrigger,
        .listInventoryParameterSpec = &InventoryParameterSpec,
    };


    // 
    // This specifies the report we want from the Reader each time the RO
    // finishes.
    //

    LLRP_tSTagReportContentSelector TagReportContentSelector = {
        .hdr.elementHdr.pType   = &LLRP_tdTagReportContentSelector,
        .EnableROSpecID         = 0,
        .EnableSpecIndex        = 0,
        .EnableInventoryParameterSpecID = 0,
        .EnableAntennaID        = 1,
        .EnableChannelIndex     = 0,
        .EnablePeakRSSI         = 1,
        .EnableFirstSeenTimestamp = 0,
        .EnableLastSeenTimestamp = 0,
        .EnableTagSeenCount     = 0,
        .EnableAccessSpecID     = 0,
    };

    LLRP_tSROReportSpec ROReportSpec;
    LLRP_tSROReportSpec * pROReportSpec = &ROReportSpec;
    if (simple_report) {
	pROReportSpec = getConfiguredROReportSpec();
    } else {
	ROReportSpec.hdr.elementHdr.pType   = &LLRP_tdROReportSpec;
	ROReportSpec.eROReportTrigger       = LLRP_ROReportTriggerType_Upon_N_Tags_Or_End_Of_ROSpec;
	ROReportSpec.N = 1;
	ROReportSpec.pTagReportContentSelector = &TagReportContentSelector;
    }

    LLRP_tSROSpec ROSpec = {
        .hdr.elementHdr.pType   = &LLRP_tdROSpec,
        .ROSpecID               = 123,
        .Priority               = 0,
        .eCurrentState          = LLRP_ROSpecState_Disabled,
        .pROBoundarySpec        = &ROBoundarySpec,
        .listSpecParameter      = &AISpec.hdr,
        .pROReportSpec          = pROReportSpec,
    };

    LLRP_tSADD_ROSPEC Cmd = {
        .hdr.elementHdr.pType   = &LLRP_tdADD_ROSPEC,
        .hdr.MessageID          = 201,
        .pROSpec                = &ROSpec,
    };

    LLRP_tSMessage *pRspMsg;
    LLRP_tSADD_ROSPEC_RESPONSE *pRsp;


    pRspMsg = transact(&Cmd.hdr);
    if(pRspMsg == NULL) {
        return -1;
    }

    pRsp = (LLRP_tSADD_ROSPEC_RESPONSE *) pRspMsg;
    if(checkLLRPStatus(pRsp->pLLRPStatus, "addROSpec") != 0) {
        freeMessage(pRspMsg);
        return -1;
    }

    freeMessage(pRspMsg);

    return 0;
}

