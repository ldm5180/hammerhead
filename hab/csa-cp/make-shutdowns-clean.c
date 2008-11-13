
//
// Copyright (C) 2004-2008, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G and NNC06CB40C.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of GNU General Public License version 2, as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//


#include <errno.h>
#include <stdlib.h>
#include <string.h>

#if defined(LINUX) || defined(MAC_OSX)
    #include <signal.h>
#endif

#include <glib.h>




#if defined(LINUX) || defined(MAC_OSX)
static void exit_signal_handler(int signal_number) {
    exit(0);
}
#endif


void make_shutdowns_clean(void) {

#if defined(LINUX) || defined(MAC_OSX)
    struct sigaction sa;

    sa.sa_handler = exit_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGABRT, &sa, NULL) < 0) {
        g_log("", G_LOG_LEVEL_ERROR, "sigaction(SIGABORT, ...): %s", strerror(errno));
        exit(1);
    }

    if (sigaction(SIGINT, &sa, NULL) < 0) {
        g_log("", G_LOG_LEVEL_ERROR, "sigaction(SIGINT, ...): %s", strerror(errno));
        exit(1);
    }

    if (sigaction(SIGTERM, &sa, NULL) < 0) {
        g_log("", G_LOG_LEVEL_ERROR, "sigaction(SIGTERM, ...): %s", strerror(errno));
        exit(1);
    }
#endif

}

