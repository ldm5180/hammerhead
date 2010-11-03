
//Impinj Tag Direction Reporting
//Added by TFK 12/22/2009


#include <errno.h>
#include <stdio.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"
//#include "out_impinj_ltkc.h"


LLRP_tSROReportSpec* getConfiguredROReportSpec(void) {
	//
	//The prototype ROSpec (without direction reporting) from the old LLRP C Toolkit Example
	/*  
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
        .N = 1, //adjusted this per the Impinj Document: SO_AN_IPJ_tag_direction... p. 6
	.pTagReportContentSelector = &TagReportContentSelector,
    };
	*/
	
	//The new version of the ROSpecReport with appropriate constructors, getters, and setters
	//
	//Instantiate & setup ROReportSpec parameter
	LLRP_tSROReportSpec      *pRORS = LLRP_ROReportSpec_construct();
	LLRP_ROReportSpec_setROReportTrigger(pRORS, LLRP_ROReportTriggerType_Upon_N_Tags_Or_End_Of_ROSpec);	
    LLRP_ROReportSpec_setN(pRORS, 0); //changed N to 1, similar to that in addROSpec function

	//Instantiate & setup TagReportContentSelector
	LLRP_tSTagReportContentSelector *pTRCS = LLRP_TagReportContentSelector_construct();
	LLRP_TagReportContentSelector_setEnableROSpecID(pTRCS, 0);
	LLRP_TagReportContentSelector_setEnableSpecIndex(pTRCS, 0);
	LLRP_TagReportContentSelector_setEnableInventoryParameterSpecID(pTRCS, 0);
	LLRP_TagReportContentSelector_setEnableAntennaID(pTRCS, 1);
	LLRP_TagReportContentSelector_setEnableChannelIndex(pTRCS, 0);
	LLRP_TagReportContentSelector_setEnablePeakRSSI(pTRCS, 1);
	LLRP_TagReportContentSelector_setEnableFirstSeenTimestamp(pTRCS, 0);
	LLRP_TagReportContentSelector_setEnableLastSeenTimestamp(pTRCS, 0);
	LLRP_TagReportContentSelector_setEnableTagSeenCount(pTRCS, 0);
	LLRP_TagReportContentSelector_setEnableAccessSpecID(pTRCS, 0);

	//add content selector to ROReportSpec
	LLRP_ROReportSpec_setTagReportContentSelector(pRORS, pTRCS);

#if 0
	//Here's where the direction reporting is added, otherwise this produces a standard ROReportSpec
	if (tagDirReport == (llrp_u16_t) 1) {
		//Instantiate & setup the Impinj tag direction reporting parameter
		//See p. 79 of the LTK Programmer's Guide for details
		LLRP_tSImpinjTagDirectionReporting *pImpTagDir = LLRP_ImpinjTagDirectionReporting_construct();
		LLRP_ImpinjTagDirectionReporting_setEnableTagDirection(pImpTagDir,1);
		LLRP_ImpinjTagDirectionReporting_setAntennaConfiguration(pImpTagDir,LLRP_ImpinjTagDirectionAntennaConfiguration_Quad_Antenna);
		
		LLRP_ROReportSpec_addCustom(pRORS,&pImpTagDir->hdr);
	}
#endif

	return pRORS;

}
