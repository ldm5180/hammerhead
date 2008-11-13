
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

#include "serial.h"




//  
//  According to the documentation, the PC transmits 0x11 and looks for
//  0x22 from the instrument.
//
//  So let's try that.
//


//
// Tries to ping the CSA-CP, return TRUE (1) if it's there, FALSE (0) if not.
//

int csa_cp_ping(serial_handle_t serial_handle) {
    int r;
    char c;


    //
    // Send the little ping command.
    //

    c = 0x11;
    r = serial_write(serial_handle, &c, 1);
    if (r < 0) {
        return 0;
    }


    // 
    // Give the CSA-CP 100 ms to respond.  (This has been experimentally
    // determined to be a reasonable timeout.)
    //

    r = serial_read(serial_handle, &c, 1, (100 * 1000));
    if (r != 1) {
        return 0;
    }

    // proper CSA-CP ping-response?
    if (c == 0x22) {
        return 1;
    }

    return 0;
}

