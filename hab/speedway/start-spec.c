
// Copyright (c) 2008-2009, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"

/*
 * Start the ROSpec using START_ROSPEC message.
 *
 * The message being sent is:
 * 	<START_ROSPEC MessageID='202'>
 *		<ROSpecID>123</ROSpecID>
 * 	</START_ROSPEC>
 */

int startROSpec() {
	
	LLRP_tSSTART_ROSPEC Cmd = {
		.hdr.elementHdr.pType = &LLRP_tdSTART_ROSPEC,
		.hdr.MessageID = 202,
		.ROSpecID = 123,
	};

	LLRP_tSMessage *pRspMsg;
	LLRP_tSSTART_ROSPEC_RESPONSE *pRsp;


	/*
	 * Send the message and expect a respose of a certain type.
	 */
	pRspMsg = transact(&Cmd.hdr);

	if (pRspMsg == NULL) {
		return -1;
	}


	/*
	 * Cast to a START_ROSPEC_RESPONSE message.
	 */
	pRsp = (LLRP_tSSTART_ROSPEC_RESPONSE *) pRspMsg;

	/*
	 * Check the LLRPStatus parameter.
	 */
	if (checkLLRPStatus(pRsp->pLLRPStatus, "startROSpec")) {
		freeMessage(pRspMsg);
		return -1;
	}

	freeMessage(pRspMsg);

	return 0;
}


