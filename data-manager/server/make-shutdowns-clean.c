
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#define _GNU_SOURCE

#include "bionet-data-manager.h"

#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

static GMutex * bdm_shutdown_mux = NULL;
static GCond * bdm_shutdown_cond = NULL;
extern GMainLoop * sync_sender_main_loop;

static void exit_signal_handler(int signal_number) {
    //g_message("caught signal %d (%s), exiting\n", signal_number, strsignal(signal_number));

    if(bdm_shutdown_mux && bdm_shutdown_cond) {
        g_mutex_lock (bdm_shutdown_mux);
        bdm_shutdown_now = 1;
        g_cond_signal(bdm_shutdown_cond);
        g_mutex_unlock (bdm_shutdown_mux);
    } else {
        bdm_shutdown_now = 1;
    }
#if ENABLE_ION
    if(bdm_bp_funcs.bp_interrupt) {
        (*bdm_bp_funcs.bp_interrupt)(dtn_thread_data.ion.sap);
    }
#endif
    if (sync_sender_main_loop) {
	g_main_loop_quit(sync_sender_main_loop);
    }

    g_main_loop_quit(bdm_main_loop);

}




static void exit_handler(void) {
}




void make_shutdowns_clean(int withThreads) {
    struct sigaction sa;

    atexit(exit_handler);

    // handle exit signals
    sa.sa_handler = exit_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGINT, &sa, NULL) < 0) {
        g_critical("sigaction(SIGINT, ...): %s", strerror(errno));
        exit(1);
    }

    if (sigaction(SIGTERM, &sa, NULL) < 0) {
        g_critical("sigaction(SIGTERM, ...): %s", strerror(errno));
        exit(1);
    }


    //
    // If a client dies or the connection is lost somehow, writes will
    // cause the BDM to receive SIGPIPE, and the default SIGPIPE handler
    // terminates the process.  Not so good.  So we change the handler to
    // ignore the signal, and we explicitly check for the error when we
    // need to.
    //

    sa.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &sa, NULL) < 0) {
        g_critical("error setting SIGPIPE sigaction to SIG_IGN: %s", strerror(errno));
        exit(1);
    }

    if(withThreads){
        bdm_shutdown_cond = g_cond_new();
        bdm_shutdown_mux = g_mutex_new();
    }
}

// Like sleep, but returns as soon as the bdm wants to shutdown
int bdm_thread_sleep() {
    g_mutex_lock (bdm_shutdown_mux);
    if(!bdm_shutdown_now){
        g_cond_wait(bdm_shutdown_cond, bdm_shutdown_mux);
    }
    g_mutex_unlock (bdm_shutdown_mux);

    return bdm_shutdown_now;
}


