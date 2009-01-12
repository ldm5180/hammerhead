
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "speedway.h"

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
		printf("Error: getTheTypeRegistry failed\n");
	}
								     
	/*
	 * Connstruct the connection using a 32kb max frame size for
	 * send and receive. The connection object is valid, but not actually
	 * connected to the reader yet.
	 */ 
	pConn = LLRP_Conn_construct(pTypeRegistry, (32u * 1024u));

	if (pConn == NULL) {
		printf("Error: Conn_construct failed\n");
	}

	/*
	 * Open the connection to the reader.
	 */
	if (g_verbose) {
		printf("INFO: Connecting to %s ... \n", reader_ip);
	}

	rc = LLRP_Conn_openConnectionToReader(pConn, reader_ip);

	if (rc != 0) {
		printf("Error: speedway_connect() %s (%d)\n", 
			pConn->pConnectErrorStr, rc);

		return -3;
	}

	if (g_verbose) {
		printf("Info: Connected, checking status ... \n");
	}

	rc = checkConnectionStatus(); 

	if (rc != 0) {
		LLRP_Conn_closeConnectionToReader(pConn);
		LLRP_Conn_destruct(pConn);

		printf("Error: connectionStatus failed\n");	
	}

    return rc;
}

