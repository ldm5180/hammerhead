#include <errno.h>
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

	return 0;
}

