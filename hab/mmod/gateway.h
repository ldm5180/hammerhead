
//
// Copyright (C) 2008, Regents of the University of Colorado.
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
