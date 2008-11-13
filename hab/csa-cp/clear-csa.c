
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

