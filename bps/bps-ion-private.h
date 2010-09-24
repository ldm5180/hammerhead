#ifndef BPS_ION_PRIVATE_H
#define BPS_ION_PRIVATE_H

#include "config.h"

#if ENABLE_ION
#include "zco.h"
#include "sdr.h"
#include "bp.h"

/*************************************************************************/
/* Ion Thread interface                                                  */
/*************************************************************************/

/* Ion Recv Thread                                                       */
/*************************************************************************/
typedef struct ion_recv_thread_hdl_opaque ion_recv_thread_hdl_t;

// Start a new thread
// @param ipcfd
//   The FD onto which the thread will push bundles
ion_recv_thread_hdl_t * start_ion_recv_thread(int ipcfd, BpSAP sap);

// Stop and destroy a running thread.
// Waits for thhread to exit before returning
int stop_ion_recv_thread(ion_recv_thread_hdl_t * args);

// Pop a bundle off the receive queue. This will block if none available.
// Use fd returned by get_recv_selectable() to wait until it won't block
int pop_recv_bundle(int usrfd, BpDelivery ** dlv);

/* Ion Send Thread                                                       */
/*************************************************************************/
typedef struct ion_send_thread_hdl_opaque ion_send_thread_hdl_t;

// Start a new thread
ion_send_thread_hdl_t * start_ion_send_thread(int ipcfd, BpSAP sap);

// Stop and destroy a running thread.
// Waits for thhread to exit before returning
int stop_ion_send_thread(ion_send_thread_hdl_t * args);

// Push a bundle onto the BP Send queue. This may block
// Use fd returned by get_send_selectable() to wait until FD is writable
int push_send_bundle(int usrfd, Object bundleZco, char * dst_eid, 
        int ttl, int priority, int custody_switch);


/*************************************************************************/
/* ION dlopen interface                                                  */
/*************************************************************************/
int load_ion(void);

typedef void (*sm_set_basekey_t)(unsigned int key);
typedef int (*bp_attach_t)(void);
typedef void (*bp_interrupt_t)(BpSAP sap);
typedef int (*bp_open_t)(char *eid, BpSAP *ionsapPtr);
typedef void (*bp_close_t)(BpSAP sap);
typedef Sdr (*bp_get_sdr_t)(void);
typedef int (*bp_send_t)(BpSAP sap, int mode, char *destEid, char *reportToEid, int lifespan, int classOfService, BpCustodySwitch custodySwitch, unsigned char srrFlags, int ackRequested, BpExtendedCOS *extendedCOS, Object adu, Object *newBundle);
typedef int (*bp_receive_t)(BpSAP sap, BpDelivery *dlvBuffer, int timeoutSeconds);
typedef int (*bp_add_endpoint_t)(char *eid, char *script);
typedef void (*bp_release_delivery_t)(BpDelivery *dlvBuffer, int releaseAdu);

typedef Object (*Sdr_malloc_t)(char *file, int line, Sdr sdr, unsigned long size);
typedef void (*sdr_begin_xn_t)(Sdr sdr);
typedef void (*sdr_cancel_xn_t)(Sdr sdr);
typedef int (*sdr_end_xn_t)(Sdr sdr);
typedef void (*Sdr_write_t)(char *file, int line, Sdr sdr, Address into, char *from, int length);

typedef Object (*zco_create_t)(Sdr sdr, ZcoMedium firstExtentSourceMedium, Object firstExtentLocation, unsigned int firstExtentOffset, unsigned int firstExtentLength);
typedef int (*zco_append_extent_t)(Sdr sdr, Object zcoRef, ZcoMedium sourceMedium, Object location, unsigned int offset, unsigned int length);
typedef void (*zco_start_receiving_t)(Sdr sdr, Object zcoRef, ZcoReader *reader);
typedef int (*zco_receive_source_t)(Sdr sdr, ZcoReader *reader, unsigned int length, char *buffer);
typedef void (*zco_stop_receiving_t)(Sdr sdr, ZcoReader *reader);
typedef unsigned int (*zco_source_data_length_t)(Sdr sdr, Object zcoRef);

typedef void (*writeErrMemo_t)(char *);
typedef void (*writeErrmsgMemos_t)(void);

typedef struct {
    sm_set_basekey_t sm_set_basekey;
    bp_attach_t bp_attach;
    bp_interrupt_t bp_interrupt;
    bp_open_t bp_open;
    bp_close_t bp_close;
    bp_get_sdr_t bp_get_sdr;
    bp_send_t bp_send;
    bp_receive_t bp_receive;
    bp_add_endpoint_t bp_add_endpoint;
    bp_release_delivery_t bp_release_delivery;

    Sdr_malloc_t Sdr_malloc;
    sdr_begin_xn_t sdr_begin_xn;
    sdr_cancel_xn_t sdr_cancel_xn;
    sdr_end_xn_t sdr_end_xn;
    Sdr_write_t Sdr_write;

    zco_create_t zco_create;
    zco_append_extent_t zco_append_extent;
    zco_start_receiving_t zco_start_receiving;
    zco_receive_source_t zco_receive_source;
    zco_stop_receiving_t zco_stop_receiving;
    zco_source_data_length_t zco_source_data_length;

    writeErrMemo_t writeErrMemo;
    writeErrmsgMemos_t writeErrmsgMemos;
} bdm_bp_funcs_t;

#define bdm_sdr_malloc(sdr, size)            (*bdm_bp_funcs.Sdr_malloc)(__FILE__, __LINE__, sdr, size)
#define bdm_sdr_write(sdr, into, from, size) (*bdm_bp_funcs.Sdr_write)(__FILE__, __LINE__, sdr, into, from, size)

extern bdm_bp_funcs_t bdm_bp_funcs;
#endif

#endif // BPS_ION_PRIVATE_H
