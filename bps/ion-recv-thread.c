
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "bps-ion-private.h"

#include <bp.h>
#include <stdio.h>

// All the options for the thread
struct ion_recv_thread_hdl_opaque {
    int thread_running;
    int ipcfd;
    BpSAP sap;

    pthread_t pthread;
};



/*
 * push a received bundle ontp the IPC socket
 *
 * Called by recv-thread
 *
 */
static int _push_recv_bundle(ion_recv_thread_hdl_t * args, BpDelivery * dlv)
{
    ssize_t bytes = send(args->ipcfd, &dlv, sizeof(BpDelivery*), MSG_NOSIGNAL);
    if (bytes != sizeof(BpDelivery*)) {
        return -1;
    }
    return 0;
}


/*
 * ion recv thread main
 *
 * This thread blocks on bp_receive(), then handles the bundle.
 *
 * Any setup is done in the user thread by acquiring a 
 */ 
static void * _recv_thread_main(void * v_args) {
    ion_recv_thread_hdl_t *args = (ion_recv_thread_hdl_t*)v_args;
    
    BpDelivery dlv;

    // Wait for bundles, and dispatch them
    while(args->thread_running) {
        if ((*bdm_bp_funcs.bp_receive)(args->sap, &dlv, BP_BLOCKING) < 0)
        {
            goto done;
        }

        switch(dlv.result) {
            case BpPayloadPresent: 
                {
                    if (_push_recv_bundle(args, &dlv)) {
                        goto done;
                    }
                }
                break;

            case BpReceptionTimedOut:
            case BpReceptionInterrupted:
                (*bdm_bp_funcs.bp_release_delivery)(&dlv, 1);
                break;

            default:
                (*bdm_bp_funcs.bp_release_delivery)(&dlv, 1);
                goto done;
        }
    }

done:
    shutdown(args->ipcfd, SHUT_WR);

    // Shutdown
    (*bdm_bp_funcs.writeErrmsgMemos)();

    return NULL;
}


/*************************************************************************
 * "Public" Methods: These are all called by the user thread
 ************************************************************************/

/*
 * pop a received bundle.
 */
int pop_recv_bundle(int usrfd, BpDelivery **dlv)
{
    ssize_t bytes = recv(usrfd, dlv, sizeof(BpDelivery*), 0);
    if (bytes != sizeof(BpDelivery*)) {
        if ( bytes == 0 ) {
            return 0; //EOF
        }
        if (bytes > 0 ) {
            errno = EPROTO;
        }
        return -1;
    }

    return 1;
}


ion_recv_thread_hdl_t * start_ion_recv_thread(int ipcfd, BpSAP sap) 
{
    int r;

    ion_recv_thread_hdl_t *args = calloc(1, sizeof(ion_recv_thread_hdl_t));
    if(NULL == args) {
        return NULL;
    }

    args->ipcfd = ipcfd;
    args->sap = sap;
    args->thread_running = 1;
    r = pthread_create(&args->pthread, NULL, _recv_thread_main, args);
    if ( r )  {
        free(args);
        return NULL;
    }

    return args;
}

int stop_ion_recv_thread(ion_recv_thread_hdl_t * args)
{
    int r;

    if(args->thread_running) {
        args->thread_running = 0;
        if(bdm_bp_funcs.bp_interrupt) {
            (*bdm_bp_funcs.bp_interrupt)(args->sap);
        }
        r = pthread_join(args->pthread, NULL);
        if ( r )  return -1;
    }

    return 0;
}
