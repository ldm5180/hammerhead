
//
// Copyright (C) 2004-2008, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G and NNC06CB40C.
//


#include <errno.h>
#include <string.h>

#include <glib.h>

#include "csa-cp-hab.h"
#include "serial.h"


int get_num_files(serial_handle_t serial_handle) {
    int r;
    char reply[2];


    // flush the serial port
    r = serial_flush(serial_handle);
    if (r < 0) {
        return -1;
    }

    // send the command to request the number of recorded files
    r = serial_write_byte(serial_handle, 0x01);
    if (r < 1) {
        return -1;
    }

    // read the reply, 2 bytes
    r = serial_read(serial_handle, reply, 2, 500*1000);
    if (r < 2) {
        return -1;
    }

    g_debug("cmd 0x01 (get num files), 2-byte response: 0x%02x 0x%02x", reply[0], reply[1]);

    // but only the second byte matters
    return reply[1];
} 

