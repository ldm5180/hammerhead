
// Copyright (c) 2008-2009, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <signal.h>
#include <stdio.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"

/*
 * SIGINT handler deallocates memory involved with reader
 * scrubs configuration
 * Closes the connection and release its resources
 * Releases registry 
 */

void handle_interrupt(int sig) {
	
	printf("\n");
	scrubConfiguration();
	LLRP_Conn_closeConnectionToReader(pConn);
	LLRP_Conn_destruct(pConn);
	LLRP_TypeRegistry_destruct(pTypeRegistry);
	exit(0);
}
