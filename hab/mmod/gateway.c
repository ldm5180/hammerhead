
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#define DEBUG 0

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <glib.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <inttypes.h>

#include "gateway.h"
#include "mmodgenmsg.h"
#include "mmodaccelmsg.h"
#include "mmodsettingsmsg.h"
#include "serialsource.h"
#include "mmod_message.h"

#define AM_HEADER_LEN          8   
#define AM_HEADER_DATALEN_LOC  5
#define AM_HEADER_TYPE_LOC     7


serial_source gateway_open(const char * usb_dev)
{
    serial_source ret;
    
    ret = open_serial_source(usb_dev, 57600, 1, NULL);
		       
    return ret;
} /* gateway_open() */


int gateway_read(serial_source src)
{
    int len;
    uint8_t *packet;
    int ret = 1; 

    packet = read_serial_packet(src, &len);
    if (NULL == packet)
    {
	return 1;
    }

    if ((MMODGENMSG_SIZE == packet[AM_HEADER_DATALEN_LOC]) 
	&& (MMODGENMSG_AM_TYPE == packet[AM_HEADER_TYPE_LOC]))
    {
	ret = msg_gen_process(&packet[AM_HEADER_LEN], MMODGENMSG_SIZE);
    }
    else if ((MMODSETTINGSMSG_SIZE == packet[AM_HEADER_DATALEN_LOC]) 
	     && (MMODSETTINGSMSG_AM_TYPE == packet[AM_HEADER_TYPE_LOC]))
    {
	ret = msg_settings_process(&packet[AM_HEADER_LEN], 
				   MMODSETTINGSMSG_SIZE);
    }

    return ret;
} /* gateway_read() */
