
// Copyright (c) 2008-2009, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <glib.h>

#include "ltkc.h"
#include "speedway.h"

/*
 * This function casts pMessage and then lets LLRP_Element_destruct() do the 
 * work.
 */

int freeMessage(LLRP_tSMessage *pMessage) {

	LLRP_Element_destruct(&pMessage->elementHdr);

	return 0;
}


