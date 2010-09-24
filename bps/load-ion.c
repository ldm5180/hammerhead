
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.
#include "config.h"

#include "bps_socket.h"
#include "bps-ion-private.h"


#include <stdio.h>
#include <dlfcn.h>

#if ENABLE_ION

bdm_bp_funcs_t bdm_bp_funcs;

#define LIBBP_FILE_NAME "libbp.so.0"

int load_ion(void) {
    void *libbp_handle;
    char *error;
    static int loaded_ion = 0;

    if (loaded_ion) return 0;

    libbp_handle = dlopen("libbp.so.0", RTLD_NOW);
    if (libbp_handle == NULL) {
        fprintf(stderr, "failed to dlopen %s: %s", LIBBP_FILE_NAME, dlerror());
        return -1;
    }

    dlerror();    /* Clear any existing error */

    /* Writing: cosine = (double (*)(double)) dlsym(handle, "cos");
    would seem more natural, but the C99 standard leaves
    casting from "void *" to a function pointer undefined.
    The assignment used below is the POSIX.1-2003 (Technical
    Corrigendum 1) workaround; see the Rationale for the
    POSIX specification of dlsym(). */

    bdm_bp_funcs.sm_set_basekey = (sm_set_basekey_t)dlsym(libbp_handle, "sm_set_basekey");
    error = dlerror();
    if (error != NULL)  {
        fprintf(stderr, "couldn't find sm_set_basekey() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.bp_attach = (bp_attach_t)dlsym(libbp_handle, "bp_attach");
    error = dlerror();
    if (error != NULL)  {
        fprintf(stderr, "couldn't find bp_attach() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.bp_interrupt = (bp_interrupt_t)dlsym(libbp_handle, "bp_interrupt");
    error = dlerror();
    if (error != NULL)  {
        fprintf(stderr, "couldn't find bp_interrupt() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.bp_open = (bp_open_t)dlsym(libbp_handle, "bp_open");
    error = dlerror();
    if (error != NULL)  {
        fprintf(stderr, "couldn't find bp_open() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.bp_close = (bp_close_t)dlsym(libbp_handle, "bp_close");
    error = dlerror();
    if (error != NULL)  {
        fprintf(stderr, "couldn't find bp_close() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.bp_get_sdr = (bp_get_sdr_t)dlsym(libbp_handle, "bp_get_sdr");
    error = dlerror();
    if (error != NULL)  {
        fprintf(stderr, "couldn't find bp_get_sdr() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.bp_send = (bp_send_t)dlsym(libbp_handle, "bp_send");
    error = dlerror();
    if (error != NULL)  {
        fprintf(stderr, "couldn't find bp_send() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.bp_receive = (bp_receive_t)dlsym(libbp_handle, "bp_receive");
    error = dlerror();
    if (error != NULL)  {
        fprintf(stderr, "couldn't find bp_receive() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.bp_add_endpoint = (bp_add_endpoint_t)dlsym(libbp_handle, "bp_add_endpoint");
    error = dlerror();
    if (error != NULL)  {
        fprintf(stderr, "couldn't find bp_add_endpoint() in libbp.so: %s, ignoring error", error);
    }

    bdm_bp_funcs.bp_release_delivery = (bp_release_delivery_t)dlsym(libbp_handle, "bp_release_delivery");
    error = dlerror();
    if (error != NULL)  {
        fprintf(stderr, "couldn't find bp_release_delivery() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.Sdr_malloc = (Sdr_malloc_t)dlsym(libbp_handle, "Sdr_malloc");
    error = dlerror();
    if (error != NULL)  {
        fprintf(stderr, "couldn't find Sdr_malloc() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.sdr_begin_xn = (sdr_begin_xn_t)dlsym(libbp_handle, "sdr_begin_xn");
    error = dlerror();
    if (error != NULL)  {
        fprintf(stderr, "couldn't find sdr_begin_xn() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.sdr_cancel_xn = (sdr_cancel_xn_t)dlsym(libbp_handle, "sdr_cancel_xn");
    error = dlerror();
    if (error != NULL)  {
        fprintf(stderr, "couldn't find sdr_cancel_xn() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.sdr_end_xn = (sdr_end_xn_t)dlsym(libbp_handle, "sdr_end_xn");
    error = dlerror();
    if (error != NULL)  {
        fprintf(stderr, "couldn't find sdr_end_xn() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.Sdr_write = (Sdr_write_t)dlsym(libbp_handle, "Sdr_write");
    error = dlerror();
    if (error != NULL)  {
        fprintf(stderr, "couldn't find Sdr_write() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.zco_create = (zco_create_t)dlsym(libbp_handle, "zco_create");
    error = dlerror();
    if (error != NULL)  {
        fprintf(stderr, "couldn't find zco_create() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.zco_append_extent = (zco_append_extent_t)dlsym(libbp_handle, "zco_append_extent");
    error = dlerror();
    if (error != NULL)  {
        fprintf(stderr, "couldn't find zco_append_extent() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.zco_start_receiving = (zco_start_receiving_t)dlsym(libbp_handle, "zco_start_receiving");
    error = dlerror();
    if (error != NULL)  {
        fprintf(stderr, "couldn't find zco_start_receiving() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.zco_receive_source = (zco_receive_source_t)dlsym(libbp_handle, "zco_receive_source");
    error = dlerror();
    if (error != NULL)  {
        fprintf(stderr, "couldn't find zco_receive_source() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.zco_stop_receiving = (zco_stop_receiving_t)dlsym(libbp_handle, "zco_stop_receiving");
    error = dlerror();
    if (error != NULL)  {
        fprintf(stderr, "couldn't find zco_stop_receiving() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.zco_source_data_length = (zco_source_data_length_t)dlsym(libbp_handle, "zco_source_data_length");
    error = dlerror();
    if (error != NULL)  {
        fprintf(stderr, "couldn't find zco_source_data_length() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }


    bdm_bp_funcs.writeErrMemo = (writeErrMemo_t)dlsym(libbp_handle, "writeErrMemo");
    error = dlerror();
    if (error != NULL)  {
        fprintf(stderr, "couldn't find writeErrMemo() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    bdm_bp_funcs.writeErrmsgMemos = (writeErrmsgMemos_t)dlsym(libbp_handle, "writeErrmsgMemos");
    error = dlerror();
    if (error != NULL)  {
        fprintf(stderr, "couldn't find writeErrmsgMemos() in %s: %s", LIBBP_FILE_NAME, error);
        return -1;
    }

    loaded_ion = 1;
    return 0;
}

#endif






