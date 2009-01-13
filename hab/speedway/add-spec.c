
#include <errno.h>
#include <stdio.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"


#define RO_TRIGGER_PERIOD 6000
#define AI_DURATION 5000


/*
 *
 * Add our ROSpec using ADD_ROSPEC message
 *
 * This ROSpec waits for a START_ROSPEC message, then takes inventory on all 
 * antennas for 5 seconds.
 *
 * The tag report is generated after the ROSpec is done.
 *
 * This example is deliberately streamlined. Nothing here configures the 
 * antennas, RF, or Gen2. The current defaults are used. Remember we just 
 * reset the reader to factory defaults (above). Normally an application 
 * would be more precise in configuring the reader and in its ROSpecs.
 *
 * Experience suggests that typical ROSpecs are about double this in size.
 *
 * The message is:
 *
 *	<ADD_ROSPEC MessageID='201'>
 *  	<ROSpec>
 *      	<ROSpecID>123</ROSpecID>
 *         	<Priority>0</Priority>
 *         	<CurrentState>Disabled</CurrentState>
 *         	<ROBoundarySpec>
 *         		<ROSpecStartTrigger>
 *            		<ROSpecStartTriggerType>Null</ROSpecStartTriggerType>
 *     			</ROSpecStartTrigger>
 *           	<ROSpecStopTrigger>
 *             		<ROSpecStopTriggerType>Null</ROSpecStopTriggerType>
 *             		<DurationTriggerValue>0</DurationTriggerValue>
 *           	</ROSpecStopTrigger>
 *         	</ROBoundarySpec>
 *         	<AISpec>
 *         		<AntennaIDs>0</AntennaIDs>
 *           	<AISpecStopTrigger>
 *             		<AISpecStopTriggerType>Duration</AISpecStopTriggerType>
 *             		<DurationTrigger>5000</DurationTrigger>
 *           	</AISpecStopTrigger>
 *           	<InventoryParameterSpec>
 *             		<InventoryParameterSpecID>1234</InventoryParameterSpecID>
 *             		<ProtocolID>EPCGlobalClass1Gen2</ProtocolID>
 *           	</InventoryParameterSpec>
 *     		</AISpec>
 *  		<ROReportSpec>
 *  			<ROReportTrigger>Upon_N_Tags_Or_End_Of_ROSpec</ROReportTrigger>
 *        		<N>0</N>
 *       		<TagReportContentSelector>
 *         		 	<EnableROSpecID>0</EnableROSpecID>
 *         		 	<EnableSpecIndex>0</EnableSpecIndex>
 *         		 	<EnableInventoryParameterSpecID>0</EnableInventoryParameterSpecID>
 *         		 	<EnableAntennaID>0</EnableAntennaID>
 *         		 	<EnableChannelIndex>0</EnableChannelIndex>
 *         		 	<EnablePeakRSSI>0</EnablePeakRSSI>
 *         		 	<EnableFirstSeenTimestamp>0</EnableFirstSeenTimestamp>
 *         		 	<EnableLastSeenTimestamp>0</EnableLastSeenTimestamp>
 *         		 	<EnableTagSeenCount>0</EnableTagSeenCount>
 *         		 	<EnableAccessSpecID>0</EnableAccessSpecID>
 *      		</TagReportContentSelector>
 *   		</ROReportSpec>
 * 		</ROSpec>
 *	</ADD_ROSPEC>
 */

int addROSpec(void) 
{

    // 
    // The RO starts periodically and doesnt stop until we tell it to stop.
    //

    LLRP_tSROSpecStartTrigger ROSpecStartTrigger = {
        .hdr.elementHdr.pType   = &LLRP_tdROSpecStartTrigger,
        .eROSpecStartTriggerType = LLRP_ROSpecStartTriggerType_Periodic,
    };

    LLRP_tSPeriodicTriggerValue PeriodicTriggerValue = {
        .hdr.elementHdr.pType = &LLRP_tdPeriodicTriggerValue,
        .Period = RO_TRIGGER_PERIOD,
        .pUTCTimestamp = NULL
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

    LLRP_tSAISpecStopTrigger    AISpecStopTrigger = {
        .hdr.elementHdr.pType   = &LLRP_tdAISpecStopTrigger,
        .eAISpecStopTriggerType = LLRP_AISpecStopTriggerType_Duration,
        .DurationTrigger        = AI_DURATION,
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

    LLRP_tSROReportSpec ROReportSpec = {
        .hdr.elementHdr.pType   = &LLRP_tdROReportSpec,
        .eROReportTrigger       = LLRP_ROReportTriggerType_Upon_N_Tags_Or_End_Of_ROSpec,
        .N = 0, 
	.pTagReportContentSelector = &TagReportContentSelector,
    };

    LLRP_tSROSpec ROSpec = {
        .hdr.elementHdr.pType   = &LLRP_tdROSpec,
        .ROSpecID               = 123,
        .Priority               = 0,
        .eCurrentState          = LLRP_ROSpecState_Disabled,
        .pROBoundarySpec        = &ROBoundarySpec,
        .listSpecParameter      = &AISpec.hdr,
        .pROReportSpec          = &ROReportSpec,
    };

    LLRP_tSADD_ROSPEC Cmd = {
        .hdr.elementHdr.pType   = &LLRP_tdADD_ROSPEC,
        .hdr.MessageID          = 201,
        .pROSpec                = &ROSpec,
    };

    LLRP_tSMessage *pRspMsg;
    LLRP_tSADD_ROSPEC_RESPONSE *pRsp;
    LLRP_tResultCode r;
    
    r = LLRP_ROSpecStartTrigger_setPeriodicTriggerValue(
        &ROSpecStartTrigger,
        &PeriodicTriggerValue
    );
    if (r != LLRP_RC_OK) {
        g_warning("error setting period trigger");
        return -1;
    }

    /*
     * Send the message, expect the response of certain type
     */
    pRspMsg = transact(&Cmd.hdr);

    if(pRspMsg == NULL) {
        return -1;
    }

    /*
     * Cast to an ADD_ROSPEC_RESPONSE message.
     */
    pRsp = (LLRP_tSADD_ROSPEC_RESPONSE *) pRspMsg;

    /*
     * Check the LLRPStatus parameter.
     */
    if(checkLLRPStatus(pRsp->pLLRPStatus, "addROSpec") != 0) {
        freeMessage(pRspMsg);

        return -1;
    }

    /*
     * Done with the response message.
     */
    freeMessage(pRspMsg);

    if(g_verbose) {
        printf("INFO: ROSpec added\n");
    }

    return 0;
}

