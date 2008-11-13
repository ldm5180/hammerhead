
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

