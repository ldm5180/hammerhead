
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "bps-ion-private.h"

#include <bp.h>
#include <errno.h>

// All the options for the thread
struct ion_send_thread_hdl_opaque {
    int thread_running;
    int ipcfd;
    BpSAP sap;
    int ttl;

    pthread_t pthread;
};

// A pointer to this is sent over the IPC socket
typedef struct {
    Object bundleZco;
    char * dst_eid;
    int ttl;
    int priority;
    int custody_switch;
} send_dgram_t;

/*
 * pop a received bundle.
 */
static int _pop_send_bundle(ion_send_thread_hdl_t * args, send_dgram_t ** dgram)
{
    ssize_t bytes = recv(args->ipcfd, dgram, sizeof(send_dgram_t*), 0);
    if (bytes != sizeof(send_dgram_t*)) {
        if ( bytes == 0 ) {
            return 0; // EOF
        }
        return -1;
    }

    return 1;
}

/*
 * Use ION to deliver the bundle
 */
static int _send_bundle_zco(ion_send_thread_hdl_t * args, send_dgram_t * dgram) 
{
    int r;
    Object unused_new_bundle;

    if ( dgram->ttl <= 0 ) {
        errno = EINVAL;
        return -1;
    }
    if ( args->sap == 0 ) {
        errno = EINVAL;
        return -1;
    }
    if ( dgram->dst_eid == NULL || dgram->dst_eid[0] == '\0' ) {
        errno = EINVAL;
        return -1;
    }

    r = (*bdm_bp_funcs.bp_send)(
        args->sap,
        BP_BLOCKING,
        dgram->dst_eid,
        NULL,                     // report-to EID
        dgram->ttl,               // Lifetime in seconds
        dgram->priority,          // class of service
        dgram->custody_switch,  
        0,                        // reporting flags - all disabled
        0,                        // app-level ack requested - what's this doing in BP?!
        NULL,                     // extended CoS - not used when CoS is STD_PRIORITY as above
        dgram->bundleZco,
        &unused_new_bundle        // handle to the bundle in the BA, we dont need it (wish we could pass in NULL here)
    );

    switch (r) {
        case 0: 
            if (errno == 0) {
                // Bad destionation eid
                errno = EDESTADDRREQ;
            } else {
                // errno is valid
            }
            return -1;

        case -1: 
            errno = ECONNREFUSED;
            return -1;

        default:
            return 0;
    }
}

/*
 * ion send thread main
 *
 * This thread blocks on bp_receive(), then handles the bundle.
 *
 * Any setup is done in the user thread by acquiring a 
 */ 
static void * _send_thread_main(void * v_args) {
    int r;

    ion_send_thread_hdl_t *args = (ion_send_thread_hdl_t*)v_args;
    
    // Wait for bundles, and dispatch them
    while(args->thread_running) {
        send_dgram_t *dgram;

        // This will block
        r = _pop_send_bundle(args, &dgram);
        if ( r == 0) {
            args->thread_running = 0;
            continue;
        }
        if ( r < 0 ){
            continue;
        }

        // This may block...
        int ttl = dgram->ttl;
        if ( ttl <= 0 ) {
            ttl = args->ttl;
        }
        r = _send_bundle_zco(args, dgram);
        int errno_save = errno;
        free(dgram->dst_eid);
        free(dgram);
        if ( r ) {
            errno = errno_save;
            goto shutdown;
        }
    }

shutdown:
    // Shutdown
    shutdown(args->ipcfd, SHUT_RD);
    (*bdm_bp_funcs.writeErrmsgMemos)();

    return NULL;
}


/*************************************************************************
 * "Public" Methods: These are all called by the user thread
 ************************************************************************/

/*
 * Push a bundle to this thread to send
 */
int push_send_bundle(int usrfd, Object bundleZco, char * dst_eid,
        int ttl, int priority, int custody_switch)
{
    send_dgram_t * dgram = malloc(sizeof(send_dgram_t));
    if(dgram == NULL){
        return -1;
    }
    dgram->bundleZco = bundleZco;
    dgram->dst_eid = strdup(dst_eid);
    dgram->ttl = ttl;
    dgram->priority = priority;
    dgram->custody_switch = custody_switch;

    ssize_t bytes = send(usrfd, &dgram, sizeof(send_dgram_t*), MSG_NOSIGNAL);
    if (bytes != sizeof(send_dgram_t*)) {
        return -1;
    }
    return 0;
}



ion_send_thread_hdl_t * start_ion_send_thread(int ipcfd, BpSAP sap) 
{
    int r;

    ion_send_thread_hdl_t *args = calloc(1, sizeof(ion_send_thread_hdl_t));
    if(NULL == args) {
        return NULL;
    }

    args->ipcfd = ipcfd;
    args->sap = sap;
    args->thread_running = 1;
    r = pthread_create(&args->pthread, NULL, _send_thread_main, args);
    if ( r )  {
        free(args);
        return NULL;
    }

    return args;
}

int stop_ion_send_thread(ion_send_thread_hdl_t * args)
{
    int r;

    if(args->thread_running) {
        args->thread_running = 0;

        r = pthread_join(args->pthread, NULL);
        if ( r )  return -1;
    }

    return 0;
}
