
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#ifndef BPS_PRIVATE_H
#define BPS_PRIVATE_H

#include "bps_socket.h"

#include "bps-ion-private.h"

#define BPS_DEFAULT_TTL  60
#define BPS_DEFAULT_CLASS_OF_SERVICE 1
#define BPS_DEFAULT_CUSTODY_SWITCH 0

// Private API used by bps wrppers for different 
// BP implementations
//
// TODO: There is some ION polition here that needs to be resolved

struct bps_sockopts {
    int bundleLifetime; 
    int classOfService; 
    int custodySwitch; 
};

typedef struct {
    int libfd; // The sockfd used by the library
    int usrfd; // The sockfd used by the library user

    ion_recv_thread_hdl_t * recv_thread;
    ion_send_thread_hdl_t * send_thread;

    int listening; // If set, then bps_recv* should fail on this socket
    int have_local_uri; // If set, then local_uri is valid
    int have_remote_uri; // If set, then remote_uri is valid

    BpDelivery * bundle; // If set, this is an accept socket
    BpSAP sap; // Set when have_local_uri is true

    struct bps_sockopts opts;

    // Only valid for accept socket
    struct {
        ZcoReader reader;
        size_t total_len;
        Object bundleZco; 
    } accept;

    // Valid for all sockets that can send
    struct {
        Object bundleZco; 
        int bundle_size;
    } send;

    
    char local_uri[BPS_EID_SIZE];
    char remote_uri[BPS_EID_SIZE];

} bps_socket_t;

#endif /*  BPS_PRIVATE_H */
