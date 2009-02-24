
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>

#include <glib.h>

#include "csa-cp-hab.h"
#include "serial.h"


serial_handle_t csa_cp_probe(const char *device_file) {
    serial_handle_t serial_handle;

    // 
    // Open the serial port.
    // FIXME: should specify RS-232 config here (baud rate, data bits, parity, and stop bits)
    //

    serial_handle = serial_open(device_file);
    if (serial_handle == INVALID_HANDLE_VALUE) {
        return INVALID_HANDLE_VALUE;
    }


    //
    // Send the little ping command.
    //

    if (!csa_cp_ping(serial_handle)) {
        serial_close(serial_handle);
        errno = ENODEV;
        return INVALID_HANDLE_VALUE;
    }

    return serial_handle;
}

