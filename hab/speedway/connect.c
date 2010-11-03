
// Copyright (c) 2008-2010, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "speedway.h"

#if 0
#include "out_impinj_ltkc.h"
#endif

/*
 * The steps done here are:
 * 	- Instantiate the connection.
 * 	- Connect to the LLRP reader (TCP).
 * 	- Make sure the connection status is good.
 */

int speedway_connect(const char* reader_ip) {
	int rc;
	
    /*
  	 *      
	 * Allocate the type registry which is needed by the connection
	 * to decode.
	 * 
	 */
	pTypeRegistry = LLRP_getTheTypeRegistry();

	if (pTypeRegistry == NULL) {
		g_warning("getTheTypeRegistry failed");
                return -1;
	}
	
#if 0
	/* Enroll the Impinj extensions. */
	LLRP_enrollImpinjTypesIntoRegistry(pTypeRegistry);
#endif
		
	/*
	 * Connstruct the connection using a 32kb max frame size for
	 * send and receive. The connection object is valid, but not actually
	 * connected to the reader yet.
	 */ 
	pConn = LLRP_Conn_construct(pTypeRegistry, (32u * 1024u));

	if (pConn == NULL) {
		g_warning("Conn_construct failed");
                return -1;
	}

	/*
	 * Open the connection to the reader.
	 */

	rc = LLRP_Conn_openConnectionToReader(pConn, reader_ip);
	if (rc != 0) {
		g_warning("speedway_connect() %s (%d)", pConn->pConnectErrorStr, rc);
		return -3;
	}

	rc = checkConnectionStatus(); 
	if (rc != 0) {
		LLRP_Conn_closeConnectionToReader(pConn);
		LLRP_Conn_destruct(pConn);
		g_warning("connectionStatus failed");	
	}

    return rc;
}

