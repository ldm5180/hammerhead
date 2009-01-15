
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#if defined(LINUX) || defined(MAC_OSX)
    #include <signal.h>
#endif

#include <glib.h>

#include "uwb.h"

#if defined(LINUX) || defined(MAC_OSX)
static void exit_signal_handler(int signal_number) {
    exit(0);
}
#endif


void make_shutdowns_clean(void) {

#if defined(LINUX) || defined(MAC_OSX)
/*
    struct sigaction sa;

    sa.sa_handler = exit_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGABRT, &sa, NULL) < 0) {
        g_log("", G_LOG_LEVEL_ERROR, "sigaction(SIGABORT, ...): %s", 
			strerror(errno));

        exit(1);
    }

    if (sigaction(SIGINT, &sa, NULL) < 0) {
        g_log("", G_LOG_LEVEL_ERROR, "sigaction(SIGINT, ...): %s", 
			strerror(errno));

        exit(1);
    }

    if (sigaction(SIGTERM, &sa, NULL) < 0) {
        g_log("", G_LOG_LEVEL_ERROR, "sigaction(SIGTERM, ...): %s", 
			strerror(errno));

        exit(1);
    }
*/
#endif
}

