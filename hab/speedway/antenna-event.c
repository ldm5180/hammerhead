
// Copyright (c) 2008-2010, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"

/*
 * Handle an AntennaEvent.
 *
 * An antenna was disconnected or (re)connected. 
 */

void handleAntennaEvent(LLRP_tSAntennaEvent *pAntennaEvent)
{
    char *pStateStr;
    llrp_u16_t AntennaID;
    LLRP_tEAntennaEventType eEventType;

    eEventType = LLRP_AntennaEvent_getEventType(pAntennaEvent);
    AntennaID = LLRP_AntennaEvent_getAntennaID(pAntennaEvent);

    switch(eEventType) {
	    case LLRP_AntennaEventType_Antenna_Disconnected:
   			pStateStr = "disconnected";

   	     	break;

   		case LLRP_AntennaEventType_Antenna_Connected:
        	pStateStr = "connected";

        	break;

    	default:
        	pStateStr = "?unknown-event?";

        	break;
    }
}

