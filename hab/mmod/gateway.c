
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
	return ret;
    }

    if ((MMODGENMSG_SIZE == packet[AM_HEADER_DATALEN_LOC]) 
	&& (MMODGENMSG_AM_TYPE == packet[AM_HEADER_TYPE_LOC]))
    {
	ret = msg_gen_process(&packet[AM_HEADER_LEN], MMODGENMSG_SIZE);
    }
    else if ((MMODACCELMSG_SIZE == packet[AM_HEADER_DATALEN_LOC]) 
	     && (MMODACCELMSG_AM_TYPE == packet[AM_HEADER_TYPE_LOC]))
    {
	ret = msg_accel_process(&packet[AM_HEADER_LEN], MMODACCELMSG_SIZE);
    }
    else if ((MMODSETTINGSMSG_SIZE == packet[AM_HEADER_DATALEN_LOC]) 
	     && (MMODSETTINGSMSG_AM_TYPE == packet[AM_HEADER_TYPE_LOC]))
    {
	ret = msg_settings_process(&packet[AM_HEADER_LEN], 
				   MMODSETTINGSMSG_SIZE);
    }

    return ret;
} /* gateway_read() */
