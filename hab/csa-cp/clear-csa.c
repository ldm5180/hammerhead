
//
// Copyright (C) 2004-2008, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G and NNC06CB40C.
//


#include <errno.h>
#include <string.h>

#include <glib.h>

#include "csa-cp-hab.h"
#include "serial.h"


int clear_csa(serial_handle_t serial_handle) {
    int r;
    char buf;

    r = serial_flush(serial_handle);
    if (r < 0) {
        return -1;
    }

    r = serial_write_byte(serial_handle, 0x04);
    if (r < 0) {
        return -1;
    }

    r = serial_read(serial_handle, &buf, 1, 500*1000);
    if (r < 1) {
        return -1;
    }
    if (buf != 0x04) {
        g_log("", G_LOG_LEVEL_WARNING, "unable to clear CSA-CP");
        return -1;
    }

    r = serial_read(serial_handle, &buf, 1, 500*1000);
    if (r < 1) {
        return -1;
    }
    if (buf != 0x00) {
        g_log("", G_LOG_LEVEL_WARNING, "unable to clear CSA-CP (received \'%c\' instead)", buf);
        return -1;
    }

    return 0;
}

