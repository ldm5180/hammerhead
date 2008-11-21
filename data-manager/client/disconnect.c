
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>

#include <glib.h>

#include "bdm-client.h"

extern int bdm_fd;

void bdm_disconnect() {
    if (bdm_fd < 0) return;


    // for the half-close scenario

    if (shutdown(bdm_fd, 2) != 0) {
        if (errno != ENOTCONN) {
            g_log(
                BDM_LOG_DOMAIN,
                G_LOG_LEVEL_WARNING,
                "bdm_disconnect(): error shutting down socket file descriptor: %s",
                strerror(errno)
            );
        }
    }

    if (close(bdm_fd) != 0) {
        g_log(
            BDM_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "bdm_disconnect(): error closing socket file descriptor: %s",
            strerror(errno)
        );
    }

    bdm_fd = -1;
}


