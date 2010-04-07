
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
 * Helper to print a message as XML text.
 */

void printXMLMessage(LLRP_tSMessage *pMessage) 
{
	char aBuf[100 * 1024];

	/*
	 * Convert the message to an XML string or an error message. The return
	 * value could be checked.
	 */

	LLRP_toXMLString(&pMessage->elementHdr, aBuf, sizeof aBuf);

	printf("%s", aBuf);
}

