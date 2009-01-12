#include <errno.h>
#include <glib.h>

#include "ltkc.h"
#include "speedway.h"

/*
 * Send a SET_READER_CONFIG message that resets the reader to factory defaults.
 *
 * NB: The ResetToFactoryDefault semantics vary between readers. It might 
 * have no effect because it is optional.
 *
 * The message is:
 *
 * <SET_READER_CONFIG MessageID='101'>
 *		<ResetToFactoryDefault>1</ResetToFactoryDefault>
 * </SET_READER_CONFIG>
 */

int resetConfigurationToFactoryDefaults() 
{
    LLRP_tSMessage *pRspMsg;
    LLRP_tSSET_READER_CONFIG_RESPONSE *pRsp;
    LLRP_tSSET_READER_CONFIG Cmd = {
        .hdr.elementHdr.pType   = &LLRP_tdSET_READER_CONFIG,
        .hdr.MessageID          = 101,
        .ResetToFactoryDefault  = 1
    };

    /*
     * Send the message, expect the response of certain type
     */
    pRspMsg = transact(&Cmd.hdr);

    if(pRspMsg == NULL) {
        return -1;
    }

    /*
     * Cast to a SET_READER_CONFIG_RESPONSE message.
     */
    pRsp = (LLRP_tSSET_READER_CONFIG_RESPONSE *) pRspMsg;

printf("pnt20\n");

    /*
     * Check the LLRPStatus parameter.
     */
    if(0 != checkLLRPStatus(pRsp->pLLRPStatus,
			"resetConfigurationToFactoryDefaults")) {
        freeMessage(pRspMsg);

        return -1;
    }

printf("pnt21\n");

    freeMessage(pRspMsg);

    if(g_verbose) {
        printf("INFO: Configuration reset to factory defaults\n");
    }

    return 0;
}


