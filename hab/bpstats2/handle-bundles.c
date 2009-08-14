
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include "bpstats2-hab.h"

/* ION Endpoint attachment */
Sdr      sdr;
BpSAP    sap;
char *   eid = NULL;

//
// Thread for reading bundles from ION, parsing them, and bionet publishing them.
//
pthread_t handle_bundles_thread;
void *handle_bundles(void *unused)
{
    BpDelivery dlv;
    ZcoReader reader;
    int rc;
    int bundlePayloadLength;
    char *bundlePayloadBuffer = NULL;
    int bundlePayloadBufferSize = 0;

    while(keep_running) {
        rc = bp_receive(sap, &dlv, BP_BLOCKING);
        if(rc < 0) {
            g_error("bp_receive() error, check ion.log");
        }

        // ION's version of EINTR: return 0, set dlv.
        if(dlv.result == BpReceptionInterrupted) {
            continue;
        }

        // Read the bundle into bundlePayloadBuffer, growing if needed.
        bundlePayloadLength = zco_source_data_length(sdr, dlv.adu);
        if(bundlePayloadLength + 1 > bundlePayloadBufferSize) {
            // We must grow bundlePayloadBuffer
            if(bundlePayloadBuffer != NULL) {
                free(bundlePayloadBuffer);
            }
            bundlePayloadBuffer = malloc(bundlePayloadLength + 1);
            if(bundlePayloadBuffer == NULL) {
                g_error("Couldn't buffer a %d-byte bundle.", 
                    bundlePayloadLength);
            }
            bundlePayloadBufferSize = bundlePayloadLength + 1;
        }
        sdr_begin_xn(sdr);
        zco_start_receiving(sdr, dlv.adu, &reader);
        if(zco_receive_source(sdr, &reader, bundlePayloadLength, 
                bundlePayloadBuffer) < 0) {
            sdr_cancel_xn(sdr);
            g_error("Couldn't zco_receive_source() bundle.");
        }
        zco_stop_receiving(sdr, &reader);
        if(sdr_end_xn(sdr) < 0)
        {
            g_error("Couldn't sdr_end_xn()");
        }
	if (bundlePayloadBuffer) {
            bundlePayloadBuffer[bundlePayloadLength] = '\0';
	}
        printf("Message from %s\n", dlv.bundleSourceEid);
        bp_release_delivery(&dlv, 1);

        //Publish bpstats to bionet.
        //FIXME
        printf("%s", bundlePayloadBuffer);
    }
    g_debug("handle_bundles thread returning.");
    return NULL;
}

