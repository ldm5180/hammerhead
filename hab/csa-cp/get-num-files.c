
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

