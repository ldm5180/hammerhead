
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

#ifndef GATEWAY_H
#define GATEWAY_H

#include "serialsource.h"

/**
 * Open the gateway device and initialize it for serial communications
 * 
 * @param[in] usb_dev Device to open. Must be a serial USB device.
 *
 * @return valid file descriptor if opened, otherwise -1
 */
serial_source gateway_open(const char * usb_dev);


/**
 * Read messages from the gateway
 *
 * @param[in] read_max Maximum number of messages to read, 0 means read until
 * socket is empty.
 *
 * @return num msgs read
 */
int gateway_read(serial_source src);


#endif /* GATEWAY_H */
