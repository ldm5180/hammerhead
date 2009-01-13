
#include <errno.h>
#include <stdio.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"

/*
 * Handle a ReaderExceptionEvent. 
 *
 * Something has gone wrong. There are lots of details but
 * all this does is print the message, if one.
 */

void handleReaderExceptionEvent(
	LLRP_tSReaderExceptionEvent *pReaderExceptionEvent)
{
    llrp_utf8v_t Message;

    Message = LLRP_ReaderExceptionEvent_getMessage(pReaderExceptionEvent);

    if(0 < Message.nValue && NULL != Message.pValue) {
        g_warning("ReaderException '%.*s'", Message.nValue, Message.pValue);
    } else {
        g_warning("ReaderException but no message");
    }
}


